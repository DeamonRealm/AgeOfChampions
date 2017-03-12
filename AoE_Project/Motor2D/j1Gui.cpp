#include "j1Gui.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1FileSystem.h"

//UI_Elements
#include "UI_Element.h"
#include "UI_Button.h"
#include "UI_Image.h"
#include "UI_Scroll.h"
#include "UI_String.h"
#include "UI_Text_Box.h"
#include "UI_Popup_Menu.h"
#include "UI_Fixed_Button.h"

//InGame elements
#include "BaseEntities.h"

j1Gui::j1Gui() : j1Module()
{
	name = "gui";
}

// Destructor
j1Gui::~j1Gui()
{}

// Called before render is available
bool j1Gui::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");
	bool ret = true;

	atlas_file_name = conf.child("atlas").attribute("file").as_string("");

	background_file_name = conf.child("background").attribute("file").as_string("");
	mainmenu_file_name = conf.child("menu").attribute("file").as_string("");
	standardmenu_file_name = conf.child("standard").attribute("file").as_string("");
	icons_file_name = conf.child("icons").attribute("file").as_string("");
	hud_file_name = conf.child("hud").attribute("file").as_string("");
	
	return ret;
}

// Called before the first frame
bool j1Gui::Start()
{
	//Load Atlas
	atlas = App->tex->Load(atlas_file_name.c_str());
	
	//UI_textures
	background = App->tex->Load(background_file_name.c_str());
	mainmenu = App->tex->Load(mainmenu_file_name.c_str());
	standardmenu = App->tex->Load(standardmenu_file_name.c_str());
	icons = App->tex->Load(icons_file_name.c_str());
	hud = App->tex->Load(hud_file_name.c_str());
	
	//Add textures to Ui_textures
	ui_textures.push_back(background);
	ui_textures.push_back(mainmenu);
	ui_textures.push_back(standardmenu);
	ui_textures.push_back(icons);
	ui_textures.push_back(hud);
	
	LoadCursorTypes();
	ChangeMouseTexture(DEFAULT);

	SDL_ShowCursor(0);

	return true;
}

// Update all guis
bool j1Gui::PreUpdate()
{
	bool ret = true;

	std::list<UI_Element*>::iterator item = screens.begin();
	while (item != screens.end()) {

		if (item._Ptr->_Myval->GetActiveState())
			ret = item._Ptr->_Myval->Update();

		item++;
	}

	return ret;
}

// Called after all Updates
bool j1Gui::PostUpdate()
{
	//Debug Mode ------------------------------------------
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN && ItemSelected != nullptr && ItemSelected->GetUItype() != TEXT_BOX)debug = !debug;

	//Tab Input
	if (App->input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN)
	{
		if (ItemSelected == NULL)ItemSelected = GetTabElement(screens.front(), 1);
		else if (ItemSelected->GetInputTarget() != (j1Module*)App->console)
		{
			ItemSelected = GetTabElement(screens.front(), ItemSelected->GetTabNum() + 1);
		}

	}

	if (App->input->GetKey(SDL_SCANCODE_0) == KEY_DOWN)
	{
		SDL_ShowCursor(0);
	}
	if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{ 
		SDL_ShowCursor(1);
	}
	

	// Update & draw the UI screens
	std::list<UI_Element*>::iterator item = screens.begin();
	while (item != screens.end()) {

		if (item._Ptr->_Myval->GetActiveState() && item._Ptr->_Myval->GetInputTarget()->active)
			item._Ptr->_Myval->Draw(debug);

		item++;
	}

	//draw cursor


	return true;
}

// Called before quitting
bool j1Gui::CleanUp()
{
	LOG("Freeing GUI");
	bool ret = true;

	while (screens.size() > 0)
	{

		//CleanUp the item childs
		ret = screens.back()->CleanUp();
		//Delete all item data
		screens.pop_back();

	}

	return true;
}




// =================================================================
const SDL_Texture* j1Gui::GetAtlas() const
{
	return atlas;
}

SDL_Texture * j1Gui::Get_UI_Texture(uint tex_id)
{
	if (tex_id > ui_textures.size() - 1)return nullptr;

	std::list<SDL_Texture*>::iterator item = ui_textures.begin();
	for (uint k = 0; k < tex_id; k++)
		item++;

	return item._Ptr->_Myval;
}

uint j1Gui::GetTabNumber() const
{
	return tabable_elements;
}

void j1Gui::SetTabNumber(uint new_tab_num)
{
	tabable_elements = new_tab_num;
}

UI_Element * j1Gui::GetTabElement(UI_Element* screen, uint index) const
{
	UI_Element* tab_element = nullptr;

	std::list<UI_Element*>::iterator item = screen->childs.begin();
	std::list<UI_Element*>::iterator end = screen->childs.end();

	while (item != end) {

		tab_element = GetTabElement(item._Ptr->_Myval, index);

		if (item._Ptr->_Myval->GetTabNum() == index && item._Ptr->_Myval->GetActiveState())
		{
			tab_element = item._Ptr->_Myval;
			break;
		}
		item++;

	}
	return tab_element;
}

uint j1Gui::PushScreen(const UI_Element* new_screen)
{
	screens.push_back((UI_Element*)new_screen);
	return screens.size();
}

UI_Element* j1Gui::GenerateUI_Element(UI_TYPE element_type)
{
	UI_Element* new_element = nullptr;
	switch (element_type)
	{
	case UNDEFINED:		
		new_element = new UI_Element();		
	break;

	case BUTTON:	
		new_element = new UI_Button();
		break;

	case TEXT_BOX:
		new_element = new UI_Text_Box();
		break;

	case STRING:
		new_element = new UI_String();
		break;

	case IMG:
		new_element = new UI_Image();
		break;

	case SCROLL:
		new_element = new UI_Scroll();
		break;

	case POPUP_MENU:
		new_element = new UI_Popup_menu();
		break;
	case FIXED_BUTTON:
		new_element = new UI_Fixed_Button();
		break;
	}
	
	return new_element;
}

UI_Element * j1Gui::GetActiveScreen() 
{
	std::list<UI_Element*>::iterator item = screens.begin();
	while (item._Ptr != NULL)
	{
		if (item._Ptr->_Myval->GetActiveState())return item._Ptr->_Myval;
		item++;
	}
	return nullptr;
}

uint j1Gui::CalculateUpperElement(UI_Element* parent, uint layer)
{
	if (parent == nullptr)return 0;

	std::list<UI_Element*>::iterator item = parent->childs.begin();
	std::list<UI_Element*>::iterator end = parent->childs.end();

	while (item != end) {

		layer = CalculateUpperElement(item._Ptr->_Myval, layer);

		if (item._Ptr->_Myval->GetLayer() > layer && item._Ptr->_Myval->GetActiveState() && item._Ptr->_Myval->MouseIsIn())
		{
			layer = item._Ptr->_Myval->GetLayer();
		}
		item++;

	}
	upper_element = layer;

	return uint(layer);
}

void j1Gui::ChangeMouseTexture(CURSOR_TARGET type)
{
	cursor_type = type;
	cursor_rect = cursors_rects[type];
	cursor_pivot = cursors_pivots[type];
}

void j1Gui::LoadCursorTypes()
{
	int vector_size = 8;
	cursors_rects.reserve(vector_size);
	cursors_pivots.reserve(vector_size);
	
	//Load Menu configuration data from loaded folder
	char* buffer = nullptr;
	int size = App->fs->Load("gui/ui_icons.xml", &buffer);
	pugi::xml_document icons_data;
	pugi::xml_parse_result result = icons_data.load_buffer(buffer, size);
	RELEASE(buffer);

	pugi::xml_node unit_node = icons_data.first_child().child("Mouse").first_child();
	
	SDL_Rect rect = { 0,0,0,0 };
	iPoint pivot = { 0,0 };

	int i = 0;
	while (i < vector_size)
	{
		rect = { unit_node.attribute("x").as_int(0),unit_node.attribute("y").as_int(0),unit_node.attribute("w").as_int(0),unit_node.attribute("h").as_int(0) };

		float pivx = unit_node.attribute("pX").as_float(0.0f)*rect.w;
		pivx = (pivx >(floor(pivx) + 0.5f)) ? ceil(pivx) : floor(pivx);
		pivot.x = (int)pivx;
		float pivy = unit_node.attribute("pY").as_float(0.0f)*rect.h;
		pivy = (pivy > (floor(pivy) + 0.5f)) ? ceil(pivy) : floor(pivy);
		pivot.y = (int)pivy;

		cursors_rects.push_back(rect);
		cursors_pivots.push_back(pivot);

		unit_node = unit_node.next_sibling();
		i++;
	}

}

void j1Gui::DrawMouseTexture()
{
	int x = 0, y = 0;
	App->input->GetMousePosition(x, y);
	App->render->Blit(icons, x - cursor_pivot.x - App->render->camera.x, y - cursor_pivot.y - App->render->camera.y, &cursor_rect);
}

void j1Gui::SetDefaultInputTarget(j1Module * target)
{
	default_input_target = target;
}

j1Module * j1Gui::GetDefaultInputTarget() const
{
	if (default_input_target == nullptr)return nullptr;
	return default_input_target;
}


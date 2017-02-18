#include "j1Gui.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"

//UI_Elements
#include "UI_Element.h"
#include "UI_Button.h"
#include "UI_Image.h"
#include "UI_Scroll.h"
#include "UI_String.h"
#include "UI_Text_Box.h"
#include "UI_Popup_Menu.h"

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

	return ret;
}

// Called before the first frame
bool j1Gui::Start()
{
	//Load Atlas
	atlas = App->tex->Load(atlas_file_name.c_str());

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

	// Update & draw the UI screens
	std::list<UI_Element*>::iterator item = screens.begin();
	while (item != screens.end()) {

		if (item._Ptr->_Myval->GetActiveState())
			item._Ptr->_Myval->Draw(debug);

		item++;
	}

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

void j1Gui::SetDefaultInputTarget(j1Module * target)
{
	default_input_target = target;
}

j1Module * j1Gui::GetDefaultInputTarget() const
{
	if (default_input_target == nullptr)return nullptr;
	return default_input_target;
}


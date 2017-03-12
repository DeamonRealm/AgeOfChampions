#include "j1Menu.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1FileSystem.h"
#include "j1Gui.h"
#include "j1Console.h"
#include "j1EntitiesManager.h"
#include "j1Animator.h"
#include "j1Scene.h"
#include "j1Player.h"

//UI Elements
#include "UI_Text_Box.h"
#include "UI_Button.h"
#include "UI_String.h"
#include "UI_Scroll.h"
#include "UI_Popup_Menu.h"
#include "UI_Fixed_Button.h"


j1Menu::j1Menu()
{
	name = "menu";
}

j1Menu::~j1Menu()
{
}

bool j1Menu::Awake(pugi::xml_node & config)
{
	LOG("Loading Scene");


	bool ret = true;
	return ret;
}

bool j1Menu::Start()
{
	App->gui->SetDefaultInputTarget(this);

	//Main menu screen
	menu_screen = App->gui->GenerateUI_Element(UNDEFINED);
	menu_screen->SetBox({ 0,0,App->win->screen_surface->w, App->win->screen_surface->h });
	menu_screen->Activate();
	menu_screen->SetInputTarget(this);

	//Load Menu configuration data from loaded folder
	char* buffer = nullptr;
	int size = App->fs->Load("gui/mainmenu.xml", &buffer);
	pugi::xml_document menu_data;
	pugi::xml_parse_result result = menu_data.load_buffer(buffer, size);
	RELEASE(buffer);
	
	pugi::xml_node unit_node = menu_data.first_child();

	//Main menu GUI
	main_menu_background = (UI_Image*)App->gui->GenerateUI_Element(IMG);
	main_menu_background->ChangeTextureRect({ 0,0,1366,768 });
	main_menu_background->ChangeTextureId(BACKGROUND);
	menu_screen->AddChild(main_menu_background);

	exit = (UI_Fixed_Button*)App->gui->GenerateUI_Element(FIXED_BUTTON);
	exit->SetTexFromXML(unit_node.child("exit"), 1);
	exit->SetBoxPosition(174, 674);
	exit->Activate();
	menu_screen->AddChild(exit);

	history = (UI_Fixed_Button*)App->gui->GenerateUI_Element(FIXED_BUTTON);
	history->SetTexFromXML(unit_node.child("history"), 1);
	history->SetBoxPosition(298, 208);
	history->Activate();
	menu_screen->AddChild(history);

	mapeditor = (UI_Fixed_Button*)App->gui->GenerateUI_Element(FIXED_BUTTON);
	mapeditor->SetTexFromXML(unit_node.child("mapeditor"), 1);
	mapeditor->SetBoxPosition(413, 348);
	mapeditor->Activate();
	menu_screen->AddChild(mapeditor);

	multiplayer = (UI_Fixed_Button*)App->gui->GenerateUI_Element(FIXED_BUTTON);
	multiplayer->SetTexFromXML(unit_node.child("multiplayer"), 1);
	multiplayer->SetBoxPosition(496, 273);
	multiplayer->Activate();
	menu_screen->AddChild(multiplayer);

	options = (UI_Fixed_Button*)App->gui->GenerateUI_Element(FIXED_BUTTON);
	options->SetTexFromXML(unit_node.child("options"), 1);
	options->SetBoxPosition(297, 443);
	options->Activate();
	menu_screen->AddChild(options);

	singleplayer = (UI_Fixed_Button*)App->gui->GenerateUI_Element(FIXED_BUTTON);
	singleplayer->SetTexFromXML(unit_node.child("singleplayer"),1);
	singleplayer->SetBoxPosition(537, 14);
	singleplayer->Activate();
	menu_screen->AddChild(singleplayer);

	turotial = (UI_Fixed_Button*)App->gui->GenerateUI_Element(FIXED_BUTTON);
	turotial->SetTexFromXML(unit_node.child("tutorial"), 1);
	turotial->SetBoxPosition(142, 8);
	turotial->Activate();
	menu_screen->AddChild(turotial);
	
	//Standard Match menu screen
/*	standard_match_screen = App->gui->GenerateUI_Element(UNDEFINED);
	standard_match_screen->SetBox({ 0,0,App->win->screen_surface->w, App->win->screen_surface->h });
	standard_match_screen->SetInputTarget(this);
	*/
	//Standard Match GUI
//	App->gui->PushScreen(standard_match_screen);
	App->gui->PushScreen(menu_screen);
	App->gui->CalculateUpperElement(menu_screen);
	

	return true;
}

bool j1Menu::PreUpdate()
{
	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) App->SetQuit();
	return true;
}

bool j1Menu::Update(float dt)
{
	App->gui->CalculateUpperElement(menu_screen);
	return true;
}

bool j1Menu::PostUpdate()
{
	if (SDL_ShowCursor(-1) == 0) App->gui->DrawMouseTexture();
	return true;
}

bool j1Menu::CleanUp()
{
	return true;
}

void j1Menu::GUI_Input(UI_Element * target, GUI_INPUT input)
{
	if (active == false)return;

	switch (input)
	{
	case UP_ARROW:
		break;
	case DOWN_ARROW:
		break;
	case LEFT_ARROW:
		break;
	case RIGHT_ARROW:
		break;
	case MOUSE_LEFT_BUTTON_DOWN:
		break;
	case MOUSE_LEFT_BUTTON_REPEAT:
		break;
	case MOUSE_LEFT_BUTTON_UP:
		if (target->GetUItype() == FIXED_BUTTON)
		{
			if (option_selected != nullptr)
			{
				option_selected->button_state = UP;
			}
			option_selected = (UI_Fixed_Button*) target;
		}
		if (target == singleplayer)
		{
			App->scene->Enable();
			App->player->Enable();
			CleanUp();
			Disable();
		}
		if (target == exit)
		{
			App->SetQuit();
		}
		break;
	case MOUSE_RIGHT_BUTTON:
		break;
	case MOUSE_IN: if (target->GetUItype() == BUTTON || target->GetUItype() == FIXED_BUTTON) App->gui->ChangeMouseTexture(SELECT);
				   else App->gui->ChangeMouseTexture(DEFAULT);
		break;
	case MOUSE_OUT:
		break;
	case SUPR:
		break;
	case BACKSPACE:
		break;
	case ENTER:
		break;
	case TAB:
		break;
	default:
		break;
	}
}

#include "j1Menu.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1FileSystem.h"
#include "j1Gui.h"
#include "j1EntitiesManager.h"
#include "j1Animator.h"
#include "j1Scene.h"
#include "j1Player.h"
#include "j1AI.h"
#include "j1SoundManager.h"
#include "j1FogOfWar.h"
#include "j1Map.h"
#include "j1InputManager.h"
#include "j1Audio.h"

//UI Elements
#include "UI_Button.h"
#include "UI_String.h"
#include "UI_Scroll.h"
#include "UI_Fixed_Button.h"


j1Menu::j1Menu()
{
	name = "menu";
}

j1Menu::~j1Menu()
{
}

void j1Menu::Init()
{
	active = false;
	enabled = false;
}

bool j1Menu::Enable()
{
	active = true;
	App->sound->PlayMusicAudio(MAIN_MENU_SONG);

	menu_screen->Activate();
	menu_screen->ActivateChilds();

	standardgame->Desactivate();
	standardgame->DesactivateChids();

	loadgame->Desactivate();
	loadgame->DesactivateChids();

	alphagame->Desactivate();
	alphagame_text->DesactivateChids();

	load_screen->Desactivate();

	App->render->camera.x = App->render->camera.y = 0;

	return true;
}

void j1Menu::Disable()
{
	active = false;
	enabled = false;
	menu_screen->Desactivate();
	menu_screen->DesactivateChids();
	
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
	RELEASE_ARRAY(buffer);
	
	pugi::xml_node unit_node = menu_data.first_child();

	//Main menu GUI
	main_menu_background = (UI_Image*)App->gui->GenerateUI_Element(IMG);
	main_menu_background->ChangeTextureRect({ 0,0,1366,768 });
	main_menu_background->ChangeTextureId(BACKGROUND);
	menu_screen->AddChild(main_menu_background);

	//Load screen 
	load_screen = (UI_Image*)App->gui->GenerateUI_Element(IMG);
	load_screen->ChangeTextureRect({ 0,0,1366,768 });
	load_screen->ChangeTextureId(LOAD_SCREEN);
	menu_screen->AddChild(load_screen);
	load_screen->SetLayer(30);
	load_screen->Desactivate();


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

	tutorial = (UI_Fixed_Button*)App->gui->GenerateUI_Element(FIXED_BUTTON);
	tutorial->SetTexFromXML(unit_node.child("tutorial"), 1);
	tutorial->SetBoxPosition(142, 8);
	tutorial->Activate();
	menu_screen->AddChild(tutorial);
	
	tutorial_text = (UI_String*)App->gui->GenerateUI_Element(STRING);
	tutorial_text->SetColor({ 255, 255, 255, 255 });
	tutorial_text->SetString("Tutorial");
	tutorial_text->Activate();
	tutorial_text->SetBox({ 75, 16,0,0 });
	tutorial->AddChild(tutorial_text);

	// Single Player Buttons
	standardgame = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	standardgame->SetBox({ 163,650,193,43 });
	standardgame->SetTexON({ 300,0,193,43 }, STANDARD);
	standardgame->SetTexOFF({ 493,0,193,43 }, STANDARD);
	standardgame->SetTexOVER({ 493,0,193,43 }, STANDARD);
	singleplayer->AddChild(standardgame);

	loadgame = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	loadgame->SetBox({ 406,650,193,43 });
	loadgame->SetTexON({ 300,0,193,43 }, STANDARD);
	loadgame->SetTexOFF({ 493,0,193,43 }, STANDARD);
	loadgame->SetTexOVER({ 493,0,193,43 }, STANDARD);
	singleplayer->AddChild(loadgame);

	alphagame = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	alphagame->SetBox({ 406,700,193,43 });
	alphagame->SetTexON({ 300,0,193,43 }, STANDARD);
	alphagame->SetTexOFF({ 493,0,193,43 }, STANDARD);
	alphagame->SetTexOVER({ 493,0,193,43 }, STANDARD);
	singleplayer->AddChild(alphagame);

	standardgame_text = (UI_String*)App->gui->GenerateUI_Element(STRING);
	standardgame_text->SetColor({ 255, 255, 255, 255 });
	standardgame_text->SetString("Standard Game: Default");
	standardgame_text->Activate();
	standardgame_text->SetBox({ 30, 13,0,0 });
	standardgame->AddChild(standardgame_text);

	loadgame_text = (UI_String*)App->gui->GenerateUI_Element(STRING);
	loadgame_text->SetColor({ 255, 255, 255, 255 });
	loadgame_text->SetString("Load Saved Game");
	loadgame_text->Activate();
	loadgame_text->SetBox({ 45, 13,0,0 });
	loadgame->AddChild(loadgame_text);

	alphagame_text = (UI_String*)App->gui->GenerateUI_Element(STRING);
	alphagame_text->SetColor({ 255, 255, 255, 255 });
	alphagame_text->SetString("Load Alpha Game");
	alphagame_text->Activate();
	alphagame_text->SetBox({ 45, 13,0,0 });
	alphagame->AddChild(alphagame_text);


	singleplayer->DesactivateChids();

	wiki = (UI_String*)App->gui->GenerateUI_Element(STRING);
	wiki->SetColor({ 255, 255, 255, 255 });
	wiki->SetString("Wiki");
	wiki->Activate();
	wiki->SetBox({ 58, 13,0,0 });
	history->AddChild(wiki);

	report_bug = (UI_String*)App->gui->GenerateUI_Element(STRING);
	report_bug->SetColor({ 255, 255, 255, 255 });
	report_bug->SetString("Report Bug");
	report_bug->Activate();
	report_bug->SetBox({ 45, 12,0,0 });
	multiplayer->AddChild(report_bug);

	singleplayer_text = (UI_String*)App->gui->GenerateUI_Element(STRING);
	singleplayer_text->SetColor({ 255, 255, 255, 255 });
	singleplayer_text->SetString("Single Player");
	singleplayer_text->Activate();
	singleplayer_text->SetBox({ 60, 13,0,0 });
	singleplayer->AddChild(singleplayer_text);

	

	//Standard Match menu screen
/*	standard_match_screen = App->gui->GenerateUI_Element(UNDEFINED);
	standard_match_screen->SetBox({ 0,0,App->win->screen_surface->w, App->win->screen_surface->h });
	standard_match_screen->SetInputTarget(this);
	*/
	//Standard Match GUI
//	App->gui->PushScreen(standard_match_screen);
	App->gui->PushScreen(menu_screen);
//	App->gui->CalculateUpperElement(menu_screen);
	

	//Load loading particle
	loading_texture = App->tex->Load("gui/UILoading.png");
	if (loading_texture != nullptr)
	{
		loading_particle = new Animation();
		loading_particle->SetTexture(loading_texture);
		loading_particle->AddSprite({ 0,0,18,4 }, { 0,0 });
		loading_particle->AddSprite({ 19,0,18,4 }, { 0,0 });
		loading_particle->AddSprite({ 38,0,18,4 }, { 0,0 });
		loading_particle->SetLoop(true);
		loading_particle->SetSpeed(400);
		loading_particle->Reset();
	}

	return true;
}

bool j1Menu::PreUpdate()
{
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		if (App->win->fullscreen)
		{
			SDL_SetWindowFullscreen(App->win->window, NULL);
			App->win->fullscreen = false;
		}
		else
		{
			SDL_SetWindowFullscreen(App->win->window, SDL_WINDOW_FULLSCREEN);
			App->win->fullscreen = true;
		}
	}

	if (App->input_manager->GetEvent(ESCAPE) == INPUT_DOWN) App->SetQuit();

	return true;
}

bool j1Menu::Update(float dt)
{
	App->gui->CalculateUpperElement(menu_screen);

	//VOLUME ------------------------------------
	if (App->input_manager->GetEvent(VOLUME_UP) == INPUT_DOWN)
	{
		App->audio->VolumeUp();
	}
	else if (App->input_manager->GetEvent(VOLUME_DOWN) == INPUT_DOWN)
	{
		App->audio->VolumeDown();
	}
	// ------------------------------------------

	return true;
}

bool j1Menu::PostUpdate()
{
	if (load_screen->GetActiveState() && loading_particle != nullptr)
	{
		const Sprite* current_sprite = loading_particle->GetCurrentSprite();
		if(current_sprite != nullptr)App->render->Blit(loading_particle->GetTexture(), 1283, 742, current_sprite->GetFrame());
		
		if (loading_particle->IsEnd())loading_particle->Reset();
	}

	if (SDL_ShowCursor(-1) == 0 && !load_screen->GetActiveState()) App->gui->DrawMouseTexture();
	return true;
}

bool j1Menu::CleanUp()
{
	if (loading_texture != nullptr)
	{
		App->tex->UnLoad(loading_texture);
		loading_texture = nullptr;
	}
	if (loading_particle != nullptr)
	{
		delete loading_particle;
		loading_particle = nullptr;
	}
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
	{
		if (target->GetUItype() == FIXED_BUTTON)
		{
			App->sound->PlayGUIAudio(CLICK_MAIN_MENU);
		}
	}
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
			singleplayer->ActivateChilds();
		}
		if (target == history)
		{
			ShellExecute(NULL, "open", "https://github.com/DeamonRealm/Age_of_Empires_II/wiki", NULL, NULL, SW_SHOWMAXIMIZED);
			history->button_state = UP;
		}
		if (target == multiplayer)
		{
			ShellExecute(NULL, "open", "https://github.com/DeamonRealm/Age_of_Empires_II/issues", NULL, NULL, SW_SHOWMAXIMIZED);
			multiplayer->button_state = UP;
		}
		if (target == tutorial)
		{
			ShellExecute(NULL, "open", "https://github.com/DeamonRealm/AgeOfChampions/wiki/Tutorial", NULL, NULL, SW_SHOWMAXIMIZED);
			tutorial->button_state = UP;
		}
		if (target == exit)
		{
			App->SetQuit();
		}
		if (target == standardgame)
		{
			option_selected->button_state = UP;
			menu_screen->DesactivateChids();
			load_screen->Activate();
			App->LoadDefaultGame("scene/standard_match.xml");

		}
		if (target == loadgame)
		{
			if (App->fs->Exists("save/party_file.xml"))
			{
				option_selected->button_state = UP;
				menu_screen->DesactivateChids();
				load_screen->Activate();
				App->LoadGame("party_file.xml");
			}
			else LOG("No party Saved");
		}
		if (target == alphagame)
		{
			option_selected->button_state = UP;
			menu_screen->DesactivateChids();
			load_screen->Activate();
			App->LoadDefaultGame("scene/alpha_match.xml");
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

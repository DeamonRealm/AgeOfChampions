#include "j1Scene.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Gui.h"
#include "j1Map.h"
#include "j1Console.h"

//UI Elements
#include "UI_Text_Box.h"
#include "UI_Button.h"
#include "UI_String.h"
#include "UI_Scroll.h"
#include "UI_Popup_Menu.h"


j1Scene::j1Scene() : j1Module()
{
	name = "scene";
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	for (pugi::xml_node map_tmx = config.child("map_folder"); map_tmx; map_tmx = map_tmx.next_sibling("map_folder")) {

		map_folder.push_back(std::string(map_tmx.child_value()));

	}

	bool ret = true;
	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	//Load textures
	tex_goal = App->tex->Load("textures/goal_texture.png");
	//tex_path = App->tex->Load("textures/path_texture.png");*/

	//Load the map
	Load_Current_Map();

	App->gui->SetDefaultInputTarget(this);

	//UI Scene build --------------------------------------
	scene_1_screen = App->gui->GenerateUI_Element(UNDEFINED);
	scene_1_screen->SetBox({ 0,0,App->win->screen_surface->w, App->win->screen_surface->h });
	scene_1_screen->Activate();
	scene_1_screen->SetInputTarget(this);
	
	civilization_menu = (UI_Popup_menu*)App->gui->GenerateUI_Element(POPUP_MENU);
	civilization_menu->SetBox({ 200, 480, 150, 90 });
	civilization_menu->SetTexMenu({ 0,0, 0, 0 });
	civilization_menu->SetTexSelection({ 0,0, 0, 0 });
	civilization_menu->Desactivate();
	scene_1_screen->AddChild(civilization_menu);
	
	teutones = (UI_String*)App->gui->GenerateUI_Element(STRING);
	teutones->SetColor({255,255,255,255});
	teutones->SetFont(NULL);
	teutones->SetString("Teutones");
	teutones->Activate();
	civilization_menu->AddChild(teutones);
	civilization_menu->AddOption(teutones);

	ingleses = (UI_String*)App->gui->GenerateUI_Element(STRING);
	ingleses->SetColor({ 255,255,255,255 });
	ingleses->SetFont(NULL);
	ingleses->SetString("Ingleses");
	ingleses->Activate();
	civilization_menu->AddChild(ingleses);
	civilization_menu->AddOption(ingleses);

	japoneses = (UI_String*)App->gui->GenerateUI_Element(STRING);
	japoneses->SetColor({ 255,255,255,255 });
	japoneses->SetFont(NULL);
	japoneses->SetString("Japoneses");
	japoneses->Activate();
	civilization_menu->AddChild(japoneses);
	civilization_menu->AddOption(japoneses);
	
	civilization_button = (UI_Button*)App->gui->GenerateUI_Element(BUTTON);
	civilization_button->SetBox({ 160, 400,230,60 });
	civilization_button->SetTexOFF({ 648,172,219,59 });
	civilization_button->SetTexON({ 5,116,220,59 });
	civilization_button->SetTexOVER({ 416,170,220,62 });
	scene_1_screen->AddChild(civilization_button);
	
	selected = (UI_String*)App->gui->GenerateUI_Element(STRING);
	selected->SetColor({ 255,255,255,255 });
	selected->SetFont(NULL);
	selected->SetString(civilization_menu->GetOptionSelected());
	selected->SetBox({ 210, 420, 50, 100 });
	selected->Activate();
	scene_1_screen->AddChild(selected);

	/*//Players img build
	player1_item = (UI_Image*)App->gui->GenerateUI_Element(IMG);
	player1_item->ChangeTextureRect({ 1485, 110, 72, 109 });
	player1_item->AdjustBox();
	player1_item->SetBoxPosition(50, -100);

	player1_item_2 = (UI_Image*)App->gui->GenerateUI_Element(IMG);
	player1_item_2->ChangeTextureRect({ 1485, 110, 72, 109 });
	player1_item_2->AdjustBox();
	player1_item_2->SetBoxPosition(-80, 70);
	
	//Button build
	button = (UI_Button*)App->gui->GenerateUI_Element(BUTTON);
	button->SetBox({ 50,50,230,60 });
	button->SetTexOFF({ 648,172,219,59 });
	button->SetTexON({ 5,116,220,59 });
	button->SetTexOVER({ 416,170,220,62 });
	scene_1_screen->AddChild(button);

	//TextBox build
	text_box = (UI_Text_Box*)App->gui->GenerateUI_Element(TEXT_BOX);
	text_box->SetBox({ 120,120,50,50 });
	text_box->SetTextColor({50,220,0,0});
	text_box->SetCursorSize(2, 5);
	scene_1_screen->AddChild(text_box);

	//Scroll build
	scroll = (UI_Scroll*)App->gui->GenerateUI_Element(SCROLL);
	scroll->SetScrollableItem({ 397,20 }, { 1000,880,19,20 });
	scroll->SetScrollableBack({ 400,20 }, { 985,874,13,149 });
	scroll->SetBox({ 250,250,450,350 });
	scroll->SetContentWindow({ 20,20,300,250 });
	scroll->SetScrollType(VERTICAL_INV);
	scroll->SetScrollMaxValue(500);
	scroll->AddScrollItem(player1_item);
	scene_1_screen->AddChild(scroll);


	//	Lateral Scroll build
	lateral_scroll = (UI_Scroll*)App->gui->GenerateUI_Element(SCROLL);
	lateral_scroll->SetScrollableItem({ 400,20 }, { 1000,880,19,20 });
	lateral_scroll->SetScrollableBack({ 400,20 }, { 0,9,307,15 });
	lateral_scroll->SetBox({ 250,250,850,350 });
	lateral_scroll->SetContentWindow({ 20,20,300,250 });
	lateral_scroll->SetScrollType(LATERAL_INV);
	lateral_scroll->SetScrollMaxValue(500);
	lateral_scroll->AddScrollItem(player1_item_2);
	scene_1_screen->AddChild(lateral_scroll);

	*/
	App->gui->PushScreen(scene_1_screen);
	// ----------------------------------------------------
	

	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	// Gui Upper Element ---------------------------
	App->gui->CalculateUpperElement(scene_1_screen);

	//MAP MOVEMENT-----------------------------------------
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		App->render->camera.y += SDL_ceil(500 * dt);
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		App->render->camera.y -= SDL_ceil(500 * dt);
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += SDL_ceil(500 * dt);

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= SDL_ceil(500 * dt);


	App->map->Draw();

	//PATHFINDING------------------------------------------
	/*if (App->gui->ItemSelected != nullptr && App->gui->ItemSelected->GetUItype() != UNDEFINED)return true;

	if (App->input->GetMouseButtonDown(1) == KEY_DOWN) {
		int x, y;
		App->input->GetMousePosition(x, y);
		App->pathfinding->SetPathStart(App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y));
	}

	if (App->input->GetMouseButtonDown(3) == KEY_DOWN) {
		int x, y;
		App->input->GetMousePosition(x, y);
		App->pathfinding->SetPathGoal(App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y));
	}
	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
		App->pathfinding->ResetPath();

	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_REPEAT)
		App->pathfinding->PropagateBFS(App->pathfinding->start, App->pathfinding->goal);

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_REPEAT)
		App->pathfinding->PropagateDijkstra();

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_REPEAT)
		App->pathfinding->PropageteA();

	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
		App->pathfinding->CreatePath(App->pathfinding->start, App->pathfinding->goal, true);

	if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN)
		App->pathfinding->CreatePath(App->pathfinding->start, App->pathfinding->goal, true, true);

	if (App->input->GetKey(SDL_SCANCODE_7) == KEY_DOWN)
		App->pathfinding->CanReach(App->pathfinding->start, App->pathfinding->goal);

	if (App->input->GetKey(SDL_SCANCODE_8) == KEY_DOWN) {
		int last_map = current_map;
		Change_Map();
		if (last_map != current_map) {
			LOG("New map loading...");
			App->pathfinding->ResetPath();
			App->map->UnLoadMap();
			Load_Current_Map();
			LOG("New map loaded!");
		}
		else LOG("Theres only one map data");
	}*/


	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;
	
	scene_1_screen->Draw(false);

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		ret = false;
	}

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

void j1Scene::GUI_Input(UI_Element* target, GUI_INPUT input)
{
	int x, y;
	App->input->GetMouseMotion(x, y);
	switch (input)
	{
	case UP_ARROW:
		if (target == civilization_menu)
		{
			selected->SetString(civilization_menu->GetOptionSelected());
		}
		break;
	case DOWN_ARROW:
		if (target == civilization_menu)
		{
			selected->SetString(civilization_menu->GetOptionSelected());
		}
		break;
	case LEFT_ARROW:
		break;
	case RIGHT_ARROW:
		break;
	case MOUSE_LEFT_BUTTON_DOWN:
		if (target == civilization_menu)
		{
			selected->SetString(civilization_menu->GetOptionSelected());
		}
		if (target == civilization_button)
		{
			if (civilization_menu->GetActiveState()) civilization_menu->Desactivate();
			else
			{
				civilization_menu->Activate();
				App->gui->ItemSelected = civilization_menu;
			}
		}
		break;
	case MOUSE_LEFT_BUTTON_REPEAT:
		if (target == scroll)
		{
			if (scroll->MoveScroll(x,y) == false)scroll->MoveBox(x, y);
		}
		else if (target == lateral_scroll)
		{
			if (lateral_scroll->MoveScroll(x, y) == false)lateral_scroll->MoveBox(x, y);
		}
		break;
	case MOUSE_LEFT_BUTTON_UP:
		if (target == scroll)
		{
			scroll->UnselectScroll();
		}
		else if (target == lateral_scroll)
		{
			lateral_scroll->UnselectScroll();
		}
		break;
	case MOUSE_RIGHT_BUTTON:
		break;
	case BACKSPACE:
		break;
	case SUPR:
		break;
	case MOUSE_IN:
		break;
	case MOUSE_OUT:
		break;
	case ENTER:
		if (target == civilization_menu)
		{
			selected->SetString(civilization_menu->GetOptionSelected());
		}
		break;
	}
}

void j1Scene::Change_Map()
{
	if (current_map < map_folder.size() - 1)current_map++;
	else current_map = 0;
}

bool j1Scene::Load_Current_Map()
{
	bool ret = false;

	if (App->map->Load(map_folder.at(current_map).c_str())) {

		ret = true;
		/*int w, h;
		uchar* data = NULL;

		//Create Walkability Map
		if (App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetWalkabilityMap(w, h, data);

		RELEASE_ARRAY(data);

		//Create walk Cost Map
		if (App->map->CreateWalkCostMap(w, h, &data)) {
			App->pathfinding->SetWalkCostMap(w, h, data);
		}

		RELEASE_ARRAY(data);

		//Create Way Size Map
		if (App->pathfinding->CreateWaySizeMap(w, h, &data)) {

			App->pathfinding->SetWaySizeMap(w, h, data);

		}

		RELEASE_ARRAY(data);*/

	}
	return ret;
}

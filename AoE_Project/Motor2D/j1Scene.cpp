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
#include "j1EntitiesManager.h"
#include "j1Animator.h"
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
	//Map build -------------------------------------------
	Load_Current_Map();
	// ----------------------------------------------------


	// Entities Build -------------------------------------
	arbalest = App->entities_manager->GenerateUnit(UNIT_TYPE::VILLAGER); ///Don't use VILLAGER_CARRY animations are differently sorted and WILL break the code
	arbalest->SetPosition(350, 280);
	town_center = App->entities_manager->GenerateBuilding(BUILDING_TYPE::TOWN_CENTER);
	town_center->SetPosition(550, 450);
	tree = App->entities_manager->GenerateResource(RESOURCE_TYPE::TREE);
	tree->SetPosition(130, 650);
	berry_bush = App->entities_manager->GenerateResource(RESOURCE_TYPE::BERRY_BUSH);
	berry_bush->SetPosition(440, 380);
	gold_ore = App->entities_manager->GenerateResource(RESOURCE_TYPE::GOLD_ORE);
	gold_ore->SetPosition(300, 480);
	stone_ore = App->entities_manager->GenerateResource(RESOURCE_TYPE::STONE_ORE);
	stone_ore->SetPosition(200, 480);
	// ----------------------------------------------------


	//UI Scene build --------------------------------------
	App->gui->SetDefaultInputTarget(this);

	scene_1_screen = App->gui->GenerateUI_Element(UNDEFINED);
	scene_1_screen->SetBox({ 0,0,App->win->screen_surface->w, App->win->screen_surface->h });
	scene_1_screen->Activate();
	scene_1_screen->SetInputTarget(this);

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
	/*
	App->gui->CalculateUpperElement(scene_1_screen);
	if (App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
	{
		j1Timer ptimer;
		YOLO = new j1ClusterAbstraction(App->map, 10);
		LOG("TIME %f", ptimer.ReadSec());
	}
	
	if (App->input->GetKey(SDL_SCANCODE_U) == KEY_DOWN)
	{
		j1Timer ptimer;
		YOLO->CreateBFS(YOLO->graph.GetNode(4), YOLO->graph.GetNode(200));
		for (int i = 0; i < YOLO->best_path.size(); i++) {
			LOG("node pass by %i", YOLO->best_path[i]->nodeNum);
		}
		LOG("TIME %f", ptimer.ReadSec());

	}
	*/
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

	//Test unit movement animation --------------
	fPoint pos = arbalest->GetPosition();
	float speed = 95;
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		arbalest->SetAction(ACTION_TYPE::WALK);
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			arbalest->SetPosition(pos.x + speed * 0.5 * dt, pos.y + -speed * 0.5 * dt);
			arbalest->SetDirection(DIRECTION_TYPE::NORTH_EAST);
		}
		else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			arbalest->SetPosition(pos.x + -speed * 0.5 * dt, pos.y + -speed * 0.5 * dt);
			arbalest->SetDirection(DIRECTION_TYPE::NORTH_WEST);
		}
		else
		{
			arbalest->SetPosition(pos.x, pos.y + -speed * dt);
			arbalest->SetDirection(DIRECTION_TYPE::NORTH);
		}
		App->animator->UnitPlay(arbalest);
	}
	else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		arbalest->SetAction(ACTION_TYPE::WALK);
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			arbalest->SetPosition(pos.x + speed * 0.5 * dt, pos.y + speed * 0.5 * dt);
			arbalest->SetDirection(DIRECTION_TYPE::SOUTH_EAST);
		}
		else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			arbalest->SetPosition(pos.x + -speed * 0.5 * dt, pos.y + speed * 0.5 * dt);
			arbalest->SetDirection(DIRECTION_TYPE::SOUTH_WEST);
		}
		else
		{
			arbalest->SetPosition(pos.x, pos.y + speed * dt);
			arbalest->SetDirection(DIRECTION_TYPE::SOUTH);
		}
		App->animator->UnitPlay(arbalest);
	}
	else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		arbalest->SetPosition(pos.x + -speed * dt, pos.y);
		arbalest->SetAction(ACTION_TYPE::WALK);
		arbalest->SetDirection(DIRECTION_TYPE::WEST);
		App->animator->UnitPlay(arbalest);
	}
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		arbalest->SetPosition(pos.x + speed * dt, pos.y );
		arbalest->SetAction(ACTION_TYPE::WALK);
		arbalest->SetDirection(DIRECTION_TYPE::EAST);
		App->animator->UnitPlay(arbalest);
	}
	else
	{
		arbalest->SetAction(ACTION_TYPE::IDLE);
		App->animator->UnitPlay(arbalest);
	}
	// ------------------------------------------

	App->map->Draw(App->debug_mode);
	App->entities_manager->Draw();

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

	ret = App->map->Load(map_folder.at(current_map).c_str());

	return ret;
}

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
#include "j1Pathfinding.h"
#include "j1FileSystem.h"
#include "j1Player.h"
#include "Hud_GamePanel.h"
#include "j1SoundManager.h"
#include "j1AI.h"
#include "j1Menu.h"
#include "j1FogOfWar.h"

j1Scene::j1Scene() : j1Module()
{
	name = "scene";
}

// Destructor
j1Scene::~j1Scene()
{

}

void j1Scene::Enable()
{
	active = true;
	LoadScene();
	App->sound->PlayMusicAudio(SOUND_TYPE::INGAME_SONG);
}

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

	//Load Logic Map --------------------------------------
	uint width = 0;
	uint height = 0;
	j1Timer ptimer;
	App->map->CreateWalkabilityMap(width, height);
	App->pathfinding->SetMap(width, height);
	// ----------------------------------------------------

	//Generate Fog of War ---------------------------------
	App->fog_of_war->GenerateFogOfWar();
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

	//MAP MOVEMENT-----------------------------------------
	bool moved = false;
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		App->render->camera.y += SDL_ceil(1500 * dt);
		moved = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		App->render->camera.y -= SDL_ceil(1500 * dt);
		moved = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		App->render->camera.x += SDL_ceil(1500 * dt);
		moved = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		App->render->camera.x -= SDL_ceil(1500 * dt);
		moved = true;
	}

	if (moved)
	{
		App->render->CalculateCameraViewport();
		App->map->CalculateTilesInView();
	}
	
	// ------------------------------------------

	App->map->Draw(App->map_debug_mode);
	App->entities_manager->Draw();
	
	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		App->scene->Disable();
		App->player->Disable();
		App->AI->Disable();
		App->entities_manager->Disable();
		App->menu->Enable();
	}

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

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
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

bool j1Scene::LoadScene()
{
	//Load Initial Scene ----------------------------------
	std::string folder = name + "/" + "Scene_Data.xml";
	pugi::xml_document scene_doc;
	if (!App->fs->LoadXML(folder.c_str(), &scene_doc))
	{
		LOG("Error Loading Scene!");
		return false;
	}

	//Load the camera position
	pugi::xml_node camera_node = scene_doc.first_child().child("camera");
	iPoint camera_pos = App->map->MapToWorld(camera_node.attribute("pos_x").as_uint(), camera_node.attribute("pos_y").as_uint());
	App->render->camera.x = camera_pos.x;
	App->render->camera.y = -camera_pos.y;
	App->render->CalculateCameraViewport();
	App->map->CalculateTilesInView();


	pugi::xml_node entity_node = scene_doc.first_child().child("entity");

	while (entity_node != NULL)
	{
		Entity* new_entity = nullptr;

		ENTITY_TYPE type = App->animator->StrToEntityEnum(entity_node.attribute("type").as_string());

		if (type == UNIT)
		{
			UNIT_TYPE unit_type = App->animator->StrToUnitEnum(entity_node.attribute("unit_type").as_string());
			DIPLOMACY diplomacy = App->animator->StrToDiplomacyEnum(entity_node.attribute("diplomacy").as_string());
			Unit* new_unit = App->entities_manager->GenerateUnit(unit_type, diplomacy, true);
			iPoint unit_pos = App->map->MapToWorldCenter(entity_node.attribute("x_pos").as_uint(), entity_node.attribute("y_pos").as_uint());
			new_unit->SetPosition((float)unit_pos.x, (float)unit_pos.y);
			App->player->game_panel->IncressPopulation(1);
		}
		else if (type == RESOURCE)
		{
			RESOURCE_TYPE resource_type = App->animator->StrToResourceEnum(entity_node.attribute("resource_type").as_string());
			Resource* new_resource = App->entities_manager->GenerateResource(resource_type);
			iPoint resource_pos = App->map->MapToWorldCenter(entity_node.attribute("x_pos").as_uint(), entity_node.attribute("y_pos").as_uint());
			new_resource->SetPosition((float)resource_pos.x, (float)resource_pos.y);
		}
		else if (type == BUILDING)
		{
			BUILDING_TYPE building_type = App->animator->StrToBuildingEnum(entity_node.attribute("building_type").as_string());
			DIPLOMACY diplomacy = App->animator->StrToDiplomacyEnum(entity_node.attribute("diplomacy").as_string());
			Building* new_building = App->entities_manager->GenerateBuilding(building_type, diplomacy);
			iPoint building_pos = App->map->MapToWorldCenter(entity_node.attribute("x_pos").as_uint(), entity_node.attribute("y_pos").as_uint());
			new_building->SetPosition((float)building_pos.x, (float)building_pos.y);
		}

		entity_node = entity_node.next_sibling();
	}
	// ----------------------------------------------------

	return true;
}

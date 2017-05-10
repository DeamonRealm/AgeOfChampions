#define _CRT_SECURE_NO_WARNINGS

#include "j1EntitiesManager.h"

#include "j1App.h"
#include "j1Render.h"
#include "j1Animator.h"
#include "j1Input.h"
#include "j1FileSystem.h"
#include "j1Pathfinding.h"
#include "j1Map.h"
#include "p2Log.h"
#include "j1SoundManager.h"
#include "j1FogOfWar.h"
#include "Resources.h"
#include "Hud_GamePanel.h"

//Testing purposes only should be erased
#include "j1Scene.h"

//Test Action Manager
#include "j1ActionManager.h"
#include "Actions_Building.h";

//Constructors ========================
j1EntitiesManager::j1EntitiesManager()
{
	name = "entities_manager";
}

//Destructors =========================
j1EntitiesManager::~j1EntitiesManager()
{

}

//Game Loop ===========================
bool j1EntitiesManager::Awake(pugi::xml_node & config_node)
{
	return true;
}

void j1EntitiesManager::Init()
{
	active = false;
	enabled = false;
}

bool j1EntitiesManager::Enable()
{
	LOG("FRAME");
	j1Timer time;

	//Load Civilization Animations --------------
	
	if (civ_data_loaded == false) /*XML doc is loaded only one time at start process*/
	{
		//Load civilization data from loaded folder
		LOG("---- Loading %s...", "Teutons.xml");
		std::string load_folder = name + "/" + "Teutons.xml";
		if (!App->fs->LoadXML(load_folder.c_str(), &civilization_data))
		{
			LOG("Civilization animations load error!");
			return true;
		}
		civ_data_loaded = true;
	}

	//Boolean to check the correct file loads
	bool ret = true;
	//Load civilization units list
	if (current_type == NO_ENTITY || current_type == UNIT)
	{
		if (current_node == NULL)current_node = civilization_data.first_child().child("units").first_child();
		while (current_node != NULL)
		{
			if (!ret)break;
			ret = App->animator->LoadUnitBlock(current_node.attribute("xml").as_string());
			current_node = current_node.next_sibling();
			if (time.Read() > TIME_TO_ENABLE && current_node != NULL) /*Break the process that will continue in the next loop*/
			{
				current_type = UNIT;
				return false;
			}
		}
		current_type = BUILDING;
		return false;
	}
	//Load civilization buildings list
	if (current_type == BUILDING)
	{
		if (current_node == NULL)current_node = civilization_data.child("data").child("buildings").first_child();
		while (current_node != NULL)
		{
			if (!ret)break;
			ret = App->animator->LoadBuildingBlock(current_node.attribute("xml").as_string());
			current_node = current_node.next_sibling();
			if (time.Read() > TIME_TO_ENABLE && current_node != NULL) /*Break the process that will continue in the next loop*/
			{
				current_type = BUILDING;
				return false;
			}
			current_type = RESOURCE;
			return false;
		}
	}
	//Load civilization resources
	if (current_type == RESOURCE)
	{
		pugi::xml_node resource_node = civilization_data.first_child().child("resources").first_child();
		if (resource_node != NULL)ret = App->animator->LoadResourceBlock(resource_node.attribute("xml").as_string());
		else LOG("Error loading civilization Resources");

		current_type = NO_ENTITY;
	}
	// ------------------------------------------

	//Clean loaded xml
	if (civilization_data != NULL && current_type == NO_ENTITY)
	{
		civ_data_loaded = false;
		civilization_data.reset();
		if(current_node != NULL)current_node(NULL);
		//LOG("---- %s loaded in %.3f", "Teutons.xml", time.ReadSec());
		return true;
	}

	return false;
}

void j1EntitiesManager::Disable()
{
	active = false;
	enabled = false;

	//Clean all arrows
	std::list<Arrow*>::const_iterator arrows = all_arrows.begin();
	for (; arrows != all_arrows.end(); arrows++)
	{
		RELEASE(arrows._Ptr->_Myval);
	}
	//Clean Up Units List
	std::list<Unit*>::iterator units_item = units.begin();
	while (units_item != units.end())
	{
		App->buff_manager->RemoveTargetBuffs(units_item._Ptr->_Myval);
		RELEASE(units_item._Ptr->_Myval);
		units_item++;
	}
	units.clear();
	units_quadtree.Reset();

	//Clean Up resources list
	std::list<Resource*>::iterator resources_item = resources.begin();
	while (resources_item != resources.end())
	{
		RELEASE(resources_item._Ptr->_Myval);
		resources_item++;
	}
	resources.clear();
	resources_quadtree.Reset();

	//Clean Up buildings list
	std::list<Building*>::iterator buildings_item = buildings.begin();
	while (buildings_item != buildings.end())
	{
		buildings_item._Ptr->_Myval->CleanMapLogic();
		RELEASE(buildings_item._Ptr->_Myval);
		buildings_item++;
	}
	buildings.clear();
	buildings_quadtree.Reset();

	//Clean Up resoureces_defs vector
	uint size = resources_defs.size();
	for (uint k = 0; k < size; k++)
	{
		RELEASE(resources_defs[k]);
	}
	resources_defs.clear();

	//Clean Up units_defs vector
	size = ally_units_defs.size();
	for (uint k = 0; k < size; k++)
	{
		RELEASE(ally_units_defs[k]);
		RELEASE(enemy_units_defs[k]);
	}
	ally_units_defs.clear();
	enemy_units_defs.clear();

	//Clean Up buildings_defs vector
	size = ally_buildings_defs.size();
	for (uint k = 0; k < size; k++)
	{
		RELEASE(ally_buildings_defs[k]);
		RELEASE(enemy_buildings_defs[k]);
	}
	ally_buildings_defs.clear();
	enemy_buildings_defs.clear();

	//Clear all death entities lists
	death_units.clear();
	death_buildings.clear();
	death_resources.clear();

	//Clear champions list
	champions_blue.clear();
	champions_red.clear();
	
}

void j1EntitiesManager::Reset()
{
	//Clean all arrows
	std::list<Arrow*>::const_iterator arrows = all_arrows.begin();
	for (; arrows != all_arrows.end(); arrows++)
	{
		RELEASE(arrows._Ptr->_Myval);
	}
	//Clean Up Units List
	std::list<Unit*>::iterator units_item = units.begin();
	while (units_item != units.end())
	{
		App->buff_manager->RemoveTargetBuffs(units_item._Ptr->_Myval);
		RELEASE(units_item._Ptr->_Myval);
		units_item++;
	}
	units.clear();
	units_quadtree.Reset();

	//Clean Up resources list
	std::list<Resource*>::iterator resources_item = resources.begin();
	while (resources_item != resources.end())
	{
		RELEASE(resources_item._Ptr->_Myval);
		resources_item++;
	}
	resources.clear();
	resources_quadtree.Reset();

	//Clean Up buildings list
	std::list<Building*>::iterator buildings_item = buildings.begin();
	while (buildings_item != buildings.end())
	{
		buildings_item._Ptr->_Myval->CleanMapLogic();
		RELEASE(buildings_item._Ptr->_Myval);
		buildings_item++;
	}
	buildings.clear();
	buildings_quadtree.Reset();

	//Clean Up resoureces_defs vector
	uint size = resources_defs.size();
	for (uint k = 0; k < size; k++)
	{
		RELEASE(resources_defs[k]);
	}
	resources_defs.clear();

	//Clean Up units_defs vector
	size = ally_units_defs.size();
	for (uint k = 0; k < size; k++)
	{
		RELEASE(ally_units_defs[k]);
		RELEASE(enemy_units_defs[k]);
	}
	ally_units_defs.clear();
	enemy_units_defs.clear();

	//Clean Up buildings_defs vector
	size = ally_buildings_defs.size();
	for (uint k = 0; k < size; k++)
	{
		RELEASE(ally_buildings_defs[k]);
		RELEASE(enemy_buildings_defs[k]);
	}
	ally_buildings_defs.clear();
	enemy_buildings_defs.clear();

	//Clear all death entities lists
	death_units.clear();
	death_buildings.clear();
	death_resources.clear();

	//Clear champions list
	champions_blue.clear();
	champions_red.clear();

}

bool j1EntitiesManager::Start()
{

	//Built entities quad trees
	units_quadtree.SetMaxObjects(12);
	units_quadtree.SetDebugColor({ 255,0,255,255 });
	resources_quadtree.SetMaxObjects(8);
	resources_quadtree.SetDebugColor({ 0,255,255,255 });
	buildings_quadtree.SetMaxObjects(8);
	buildings_quadtree.SetDebugColor({ 255,255,0,255 });


	return true;
}

bool j1EntitiesManager::Update(float dt)
{
	bool ret = true;

	//Update alive arrows
	std::list<Arrow*>::const_iterator arrows = all_arrows.begin();
	for (; arrows != all_arrows.end(); arrows++)
	{

		if (arrows._Ptr->_Myval->Draw())
		{
			RemoveArrow(arrows._Ptr->_Myval);
			RELEASE(arrows._Ptr->_Myval);
		}
	}

	//Update all the units
	std::list<Unit*>::const_iterator item = units.begin();
	while (item != units.end())
	{
		ret = item._Ptr->_Myval->Update();
		if (item == units.end() || units.empty())break;
		item++;

	}

	//Update all the death units
	std::list<Unit*>::const_iterator death_unit = death_units.begin();
	while (death_unit != death_units.end())
	{
		ret = death_unit._Ptr->_Myval->Update();
		if (death_unit == death_units.end() || death_units.empty())break;
		death_unit++;
	}

	//Update all the units
	std::list<Building*>::const_iterator death_building = death_buildings.begin();
	while (death_building != death_buildings.end())
	{
		ret = death_building._Ptr->_Myval->Update();
		if (death_building == death_buildings.end() || death_buildings.empty())break;
		death_building++;
	}

	//Update all the buildings
	std::list<Building*>::const_iterator item_build = buildings.begin();
	while (item_build != buildings.end())
	{

		ret = item_build._Ptr->_Myval->Update();
		if (item_build == buildings.end() || buildings.empty())break;
		item_build++;
	}

	//Remove all the defeated entities of the correct lists
	//Add the entity at the wasted entities list
	//Update all the death units
	std::list<Entity*>::const_iterator wasted_enty = ready_to_waste.begin();
	while (wasted_enty != ready_to_waste.end())
	{
		if (wasted_enty._Ptr->_Myval->GetEntityType() == UNIT)
		{
			death_units.remove((Unit*)wasted_enty._Ptr->_Myval);
			units.remove((Unit*)wasted_enty._Ptr->_Myval);
		}
		else if (wasted_enty._Ptr->_Myval->GetEntityType() == BUILDING)
		{
			death_buildings.remove((Building*)wasted_enty._Ptr->_Myval);
			buildings.remove((Building*)wasted_enty._Ptr->_Myval);
		}
		else
		{
			death_resources.remove((Resource*)wasted_enty._Ptr->_Myval);
			resources.remove((Resource*)wasted_enty._Ptr->_Myval);
		}
		wasted_entities.push_back(wasted_enty._Ptr->_Myval);
		wasted_enty++;
	}
	ready_to_waste.clear();

	return ret;
}

bool j1EntitiesManager::PostUpdate()
{


	return true;
}

bool j1EntitiesManager::Draw()
{
	bool ret = true;
	//Draw all arrows
	
	//Draw all units
	std::vector<Unit*> units_vec;
	units_quadtree.CollectCandidates(units_vec, App->render->camera_viewport);
	uint size = units_vec.size();
	iPoint pos;
	FOG_TYPE fog_type;
	for (uint k = 0; k < size; k++)
	{
		pos = units_vec[k]->GetPositionRounded();
		pos = App->map->WorldToMap(pos.x, pos.y);
		fog_type = App->fog_of_war->GetFogID(pos.x, pos.y);
		if (!App->map_debug_mode)
		{
			if (fog_type == DARK_FOG)continue;
			if (units_vec[k]->GetDiplomacy() == ENEMY && fog_type != NO_FOG)continue;
		}


		units_vec[k]->Draw(App->debug_mode);
	}
	//Draw all death units
	std::list<Unit*>::const_iterator death_unit = death_units.begin();
	for (; death_unit != death_units.end(); death_unit++)
	{

		death_unit._Ptr->_Myval->Draw(App->debug_mode);

	}

	//Draw all death buildings
	std::list<Building*>::const_iterator death_building = death_buildings.begin();
	for (; death_building != death_buildings.end(); death_building++)
	{
		death_building._Ptr->_Myval->Draw(App->debug_mode);

	}

	//Update all death resources
	std::list<Resource*>::iterator death_resource = death_resources.begin();
	for (; death_resource != death_resources.end(); death_resource++)
	{
		if (!death_resource._Ptr->_Myval->Draw(App->debug_mode))
		{
			App->entities_manager->DeleteEntity(death_resource._Ptr->_Myval);
		}

	}

	//Draw all Resources
	std::vector<Resource*> resources_vec;
	resources_quadtree.CollectCandidates(resources_vec, App->render->camera_viewport);
	size = resources_vec.size();
	for (uint k = 0; k < size; k++)
	{
		pos = resources_vec[k]->GetPositionRounded();
		pos = App->map->WorldToMap(pos.x, pos.y);
		if (App->fog_of_war->GetFogID(pos.x, pos.y) != DARK_FOG || App->map_debug_mode)resources_vec[k]->Draw(App->debug_mode);
	}

	//Draw all buildings
	std::vector<Building*> buildings_vec;
	SDL_Rect buildings_rect = App->render->camera_viewport;
	buildings_rect.x -= 50;
	buildings_rect.y -= 50;
	buildings_rect.w += 100;
	buildings_rect.h += 250;
	buildings_quadtree.CollectCandidates(buildings_vec, buildings_rect);
	size = buildings_vec.size();
	for (uint k = 0; k < size; k++)
	{
		buildings_vec[k]->Draw(App->debug_mode);
	}

	//Draw Units quadtree in debug mode
	if (App->map_debug_mode)
	{
		units_quadtree.Draw();
	}

	return ret;
}

bool j1EntitiesManager::CleanUp()
{
	//Clean all arrows
	std::list<Arrow*>::const_iterator arrows = all_arrows.begin();
	for (; arrows != all_arrows.end(); arrows++)
	{
		RELEASE(arrows._Ptr->_Myval);
	}

	//Clean Up Units List
	units_quadtree.Clear();
	std::list<Unit*>::iterator units_item = units.begin();
	while (units_item != units.end())
	{
		RELEASE(units_item._Ptr->_Myval);
		units_item++;
	}
	units.clear();

	//Clean Up Resources List
	resources_quadtree.Clear();
	std::list<Resource*>::iterator resource_item = resources.begin();
	while (resource_item != resources.end())
	{
		RELEASE(resource_item._Ptr->_Myval);
		resource_item++;
	}
	resources.clear();

	//Clean Up buildings list
	buildings_quadtree.Clear();
	std::list<Building*>::iterator buildings_item = buildings.begin();
	while (buildings_item != buildings.end())
	{
		RELEASE(buildings_item._Ptr->_Myval);
		buildings_item++;
	}
	buildings.clear();

	//Clean Up resoureces_defs vector
	uint size = resources_defs.size();
	for (uint k = 0; k < size; k++)
	{
		RELEASE(resources_defs[k]);
	}
	resources_defs.clear();

	//Clean Up units_defs vector
	size = ally_units_defs.size();
	for (uint k = 0; k < size; k++)
	{
		RELEASE(ally_units_defs[k]);
		RELEASE(enemy_units_defs[k]);
	}
	ally_units_defs.clear();
	enemy_units_defs.clear();

	//Clean Up buildings_defs vector
	size = ally_buildings_defs.size();
	for (uint k = 0; k < size; k++)
	{
		RELEASE(ally_buildings_defs[k]);
		RELEASE(enemy_buildings_defs[k]);
	}
	ally_buildings_defs.clear();
	enemy_buildings_defs.clear();

	//Clean Champions lists
	champions_blue.clear();
	champions_red.clear();

	//If there's no entities to delete returns
	if (wasted_entities.empty())return true;

	//Clean all the wasted entities
	size = wasted_entities.size();
	for (uint k = 0; k < size; k++)
	{
		RELEASE(wasted_entities[k]);
	}
	wasted_entities.clear();
	
	return true;
}

bool j1EntitiesManager::Load(pugi::xml_node& data)
{
	
	// Load current entities definitions --------
	//Node where definitions are loaded
	pugi::xml_node definitions_node = data.child("definitions");

	// Resources Definitions ----------
	//Node where resources definitions are loaded
	pugi::xml_node resources_defs_node = definitions_node.child("resources");

	//Node where the current resource definition is loaded
	pugi::xml_node resource_node = resources_defs_node.child("res");

	while (resource_node != NULL)
	{

		App->entities_manager->AddResourceDefinition(&resource_node);

		resource_node = resource_node.next_sibling();
	}
	// --------------------------------

	//Buildings Definitions -----------
	//Node where buildings definitions are loaded
	pugi::xml_node buildings_defs_node = definitions_node.child("buildings");

	//Node where ally buildings definitions are loaded
	pugi::xml_node ally_buildings_node = buildings_defs_node.child("ally");
	//Node where enemy buildings definitions are loaded
	pugi::xml_node enemy_buildings_node = buildings_defs_node.child("enemy");

	//Node where current ally building def is loaded
	pugi::xml_node ally_build_node = ally_buildings_node.child("build");

	//Node where current ally building def is loaded
	pugi::xml_node enemy_build_node = enemy_buildings_node.child("build");


	//Iterate all buildings definitions
	while (ally_build_node != NULL)
	{
		AddBuildingDefinition(&ally_build_node, ALLY);
		AddBuildingDefinition(&enemy_build_node, ENEMY);

		ally_build_node = ally_build_node.next_sibling();
		enemy_build_node = enemy_build_node.next_sibling();
	}
	// --------------------------------

	//Units Definitions ---------------
	//Node where units definitions are loaded
	pugi::xml_node units_defs_node = definitions_node.child("units");

	//Node where ally units definitions are loaded
	pugi::xml_node ally_units_node = units_defs_node.child("ally");
	//Node where enemy units definitions are loaded
	pugi::xml_node enemy_units_node = units_defs_node.child("enemy");

	//Node where current ally unit def is loaded
	pugi::xml_node ally_unit_node = ally_units_node.child("unit");
	//Node where current ally unit def is loaded
	pugi::xml_node enemy_unit_node = enemy_units_node.child("unit");

	while (ally_unit_node != NULL)
	{
		AddUnitDefinition(&ally_unit_node, ALLY);
		AddUnitDefinition(&enemy_unit_node, ENEMY);

		ally_unit_node = ally_unit_node.next_sibling();
		enemy_unit_node = enemy_unit_node.next_sibling();
	}
	// --------------------------------
	// ------------------------------------------
	
	// Resources Load ---------------------------
	//Resources node
	pugi::xml_node resources_node = data.child("resources");

	//Focus the first resource node
	pugi::xml_node cur_res_node = resources_node.child("res");

	//Iterate all the saved resources
	while (cur_res_node != NULL)
	{

		/*
		- Position
		- RESOURCE_TYPE
		- life
		*/

		RESOURCE_TYPE correct_type = (RESOURCE_TYPE)cur_res_node.attribute("res_type").as_int();

		switch (correct_type)
		{
		case TREE_CUT:
		case CHOP:
			correct_type = TREE;
			break;
		case TINY_GOLD_ORE:
			correct_type = GOLD_ORE;
			break;
		case TINY_STONE_ORE:
			correct_type = STONE_ORE;
			break;
		}


		Resource* new_res = App->entities_manager->GenerateResource(correct_type);

		if (new_res == nullptr)
		{
			LOG("Resource Load Error!");
			//Focus the next resource
			cur_res_node = cur_res_node.next_sibling();
			continue;
		}
		//Load position
		new_res->SetPosition(cur_res_node.attribute("pos_x").as_float(), cur_res_node.attribute("pos_y").as_float());

		//Load life
		new_res->SetLife(cur_res_node.attribute("life").as_int());

		//Load current building selected
		if (cur_res_node.attribute("selected").as_bool() == true) new_res->Select();

		//Focus the next resource
		cur_res_node = cur_res_node.next_sibling();
	}
	// ------------------------------------------

	// Buildings Load ---------------------------
	//Vector where loaded buildings are allocated to load actions later
	std::vector<Building*> loaded_buildings;

	//Buildings node
	pugi::xml_node buildings_node = data.child("buildings");


	//Focus the first building node
	pugi::xml_node cur_build_node = buildings_node.child("build");

	//Iterate all the saved buildings
	while (cur_build_node != NULL)
	{

		//Load current building type
		BUILDING_TYPE building_type = (BUILDING_TYPE)cur_build_node.attribute("build_type").as_int();

		//Generate building from type and diplomacy
		Building* new_building = GenerateBuilding(building_type, (DIPLOMACY)cur_build_node.attribute("diplomacy").as_int());

		if (new_building == nullptr)
		{
			LOG("Error loading building!");
			//Focus the next building node
			cur_build_node = cur_build_node.next_sibling();
			continue;
		}
		//Load building position
		new_building->SetPosition(cur_build_node.attribute("pos_x").as_float(), cur_build_node.attribute("pos_y").as_float());

		//Load building life
		new_building->SetLife(cur_build_node.attribute("life").as_uint());

		//Load current building selected
		if (cur_build_node.attribute("selected").as_bool() == true) new_building->Select();

		//Add new building loaded to the loaded vector to load actions later
		loaded_buildings.push_back(new_building);

		//Focus the next building node
		cur_build_node = cur_build_node.next_sibling();
	}
	// ------------------------------------------

	// Units Load -------------------------------
	//Vector where loaded units are allocated to load actions later
	std::vector<Unit*> loaded_units;

	//Units node
	pugi::xml_node units_node = data.child("units");

	//Node where the unit is loaded
	pugi::xml_node cur_unit_node = units_node.child("unit");

	//Iterate all the saved units
	while (cur_unit_node != NULL)
	{
		/*
		- UNIT_TYPE
		- diplomacy
		- life
		- position
		*/



		//Load current unit type
		UNIT_TYPE unit_type = (UNIT_TYPE)cur_unit_node.attribute("unit_type").as_int();


		//Generate unit from unit type and diplomacy
		Unit* new_unit = GenerateUnit(unit_type, (DIPLOMACY)cur_unit_node.attribute("diplomacy").as_int());


		//Load current unit life
		new_unit->SetLife(cur_unit_node.attribute("life").as_uint());

		//Load current unit position
		new_unit->SetPosition(cur_unit_node.attribute("pos_x").as_float(), cur_unit_node.attribute("pos_y").as_float());

		//Load current unit selected
		if (cur_unit_node.attribute("selected").as_bool() == true) new_unit->Select();

		switch (unit_type)
		{
		case VILLAGER:
			/*
			- resource_collected_type
			- current_resources
			*/

			//Load collected resource type
			((Villager*)new_unit)->SetResourceCollectedType((PLAYER_RESOURCES)cur_unit_node.attribute("res_collected_type").as_int());
			//Load current resources count
			((Villager*)new_unit)->SetCurrentResources(cur_unit_node.attribute("current_res").as_uint());

			App->animator->UnitPlay(new_unit);

			break;
		case WARRIOR_CHMP:
		case ARCHER_CHMP:
		case WIZARD_CHMP:
			/*
			- level
			- ability [3]
			- actived[3];
			- ability_lvl_2_current_time
			- ability_lvl_3_current_time
			*/
			//Load champion level
			uint level = cur_unit_node.attribute("lvl").as_uint();
			((Champion*)new_unit)->SetLevel(level);
			for (uint k = 0; k < level; k++)((Champion*)new_unit)->LevelUp(false);
			bool choosed_1 = cur_unit_node.attribute("ab_choosed_1").as_bool();
			bool choosed_2 = cur_unit_node.attribute("ab_choosed_2").as_bool();
			bool choosed_3 = cur_unit_node.attribute("ab_choosed_3").as_bool();

			//Load champion abilities choosed state
			((Champion*)new_unit)->SetSkillChoosed(choosed_1, 0);
			((Champion*)new_unit)->SetSkillChoosed(choosed_2, 1);
			((Champion*)new_unit)->SetSkillChoosed(choosed_3, 2);

			//Load champion abilities chosen
			if (choosed_1)
			((Champion*)new_unit)->SetAbility_lvl_1(cur_unit_node.attribute("ability_1").as_bool());
			if (choosed_2)
			((Champion*)new_unit)->SetAbility_lvl_2(cur_unit_node.attribute("ability_2").as_bool());
			if (choosed_3)

			((Champion*)new_unit)->SetAbility_lvl_3(cur_unit_node.attribute("ability_3").as_bool());

			break;
		}

		//Save loaded units in units loaded vector 
		loaded_units.push_back(new_unit);

		//Focus the next unit node
		cur_unit_node = cur_unit_node.next_sibling();
	}

	//Load loaded units tasks ---------

	//Focus unit node to the first unit saved
	cur_unit_node = units_node.first_child();

	//Iterate all saved units again
	uint k = 0;
	while (cur_unit_node != NULL)
	{
		//Focus current unit actions node
		pugi::xml_node action_node = cur_unit_node.child("actions");

		//Focus current unit primary actions node
		pugi::xml_node primary_act_node = action_node.child("primary").first_child();
		while (primary_act_node != NULL)
		{

			App->action_manager->LoadTask(primary_act_node, (Entity*)loaded_units[k], PRIMARY);

			//Focus next primary action node
			primary_act_node = primary_act_node.next_sibling();
		}

		//Focus next saved unit node
		cur_unit_node = cur_unit_node.next_sibling();

		//Focus next saved unit
		k++;
	}
	// --------------------------------

	//Load loaded buildings tasks ---------

	//Focus building node to the first building saved
	cur_build_node = buildings_node.first_child();

	//Iterate all saved buildings again
	k = 0;
	while (cur_build_node != NULL)
	{
		//Focus current building actions node
		pugi::xml_node action_node = cur_build_node.child("actions");

		//Focus current building primary actions node
		pugi::xml_node primary_act_node = action_node.child("primary").first_child();
		while (primary_act_node != NULL)
		{

			App->action_manager->LoadTask(primary_act_node, (Entity*)loaded_buildings[k], PRIMARY);

			//Focus next primary action node
			primary_act_node = primary_act_node.next_sibling();
		}

		//Focus next saved building node
		cur_build_node = cur_build_node.next_sibling();

		//Focus next saved building
		k++;
	}
	// --------------------------------

	loaded_buildings.clear();
	loaded_units.clear();

	// ------------------------------------------

	return true;
}

bool j1EntitiesManager::Save(pugi::xml_node& data) const
{
	// Save current entities definitions --------
	//Node where definitions are saved
	pugi::xml_node definitions_node = data.append_child("definitions");

	// Resources Definitions ----------
	//Node where resources definitions are saved
	pugi::xml_node resources_defs_node = definitions_node.append_child("resources");

	//Iterate all resources definitions
	uint size = resources_defs.size();
	for (uint k = 0; k < size; k++)
	{
		//Node where the current resource definition is saved
		pugi::xml_node resource_node = resources_defs_node.append_child("res");

		//Save characteristics of the current resource node
		resources_defs[k]->SaveAsDef(resource_node);
	}
	// --------------------------------
	
	//Buildings Definitions -----------
	//Node where buildings definitions are saved
	pugi::xml_node buildings_defs_node = definitions_node.append_child("buildings");

	//Node where ally buildings definitions are saved
	pugi::xml_node ally_buildings_node = buildings_defs_node.append_child("ally");
	//Node where enemy buildings definitions are saved
	pugi::xml_node enemy_buildings_node = buildings_defs_node.append_child("enemy");

	//Iterate all buildings definitions
	size = ally_buildings_defs.size();
	for (uint k = 0; k < size; k++)
	{
		//Node where current ally building def is saved
		pugi::xml_node ally_build_node = ally_buildings_node.append_child("build");
		ally_buildings_defs[k]->SaveAsDef(ally_build_node);
		//Node where current ally building def is saved
		pugi::xml_node enemy_build_node = enemy_buildings_node.append_child("build");
		enemy_buildings_defs[k]->SaveAsDef(enemy_build_node);
	}
	// --------------------------------
	
	//Units Definitions ---------------
	//Node where units definitions are saved
	pugi::xml_node units_defs_node = definitions_node.append_child("units");

	//Node where ally units definitions are saved
	pugi::xml_node ally_units_node = units_defs_node.append_child("ally");
	//Node where enemy units definitions are saved
	pugi::xml_node enemy_units_node = units_defs_node.append_child("enemy");

	//Iterate all units definitions
	size = ally_units_defs.size();
	for (uint k = 0; k < size; k++)
	{
		//Node where current ally unit def is saved
		pugi::xml_node ally_unit_node = ally_units_node.append_child("unit");
		ally_units_defs[k]->SaveAsDef(ally_unit_node);
		//Node where current ally unit def is saved
		pugi::xml_node enemy_unit_node = enemy_units_node.append_child("unit");
		enemy_units_defs[k]->SaveAsDef(enemy_unit_node);
	}
	// --------------------------------
	// ------------------------------------------

	// Resources Save ---------------------------
	//Resources node
	pugi::xml_node resources_node = data.append_child("resources");

	//Iterate all the current resources
	std::list<Resource*>::const_iterator current_resource = resources.begin();
	while (current_resource != resources.end())
	{
		//Node where current resource is saved
		pugi::xml_node cur_res_node = resources_node.append_child("res");

		/*
		- Position
		- RESOURCE_TYPE
		- life
		*/

		//Save resource position
		cur_res_node.append_attribute("pos_x") = current_resource._Ptr->_Myval->GetPosition().x;
		cur_res_node.append_attribute("pos_y") = current_resource._Ptr->_Myval->GetPosition().y;

		//Save resource type
		cur_res_node.append_attribute("res_type") = current_resource._Ptr->_Myval->GetResourceType();

		//Save resource life
		cur_res_node.append_attribute("life") = current_resource._Ptr->_Myval->GetLife();

		//Save current resource selected
		cur_res_node.append_attribute("selected") = current_resource._Ptr->_Myval->GetIfSelected();

		//Focus the next resource
		current_resource++;
	}
	// ------------------------------------------

	// Buildings Save ---------------------------
	//Buildings node
	pugi::xml_node buildings_node = data.append_child("buildings");

	//Iterate all the current buildings
	std::list<Building*>::const_iterator current_building = buildings.begin();
	while (current_building != buildings.end())
	{
		//Node where current building is saved
		pugi::xml_node cur_build_node = buildings_node.append_child("build");

		/*
		- Position
		- BUILDING_TYPE
		- life
		*/

		//Save building type
		BUILDING_TYPE type = current_building._Ptr->_Myval->GetBuildingType();
		cur_build_node.append_attribute("build_type") = type;

		//Save building position
		fPoint pos = current_building._Ptr->_Myval->GetPosition();
		if (type == TOWN_CENTER || type == HOUSE_A || type == HOUSE_B || type == HOUSE_C)
		{
			pos.y += App->map->data.tile_height * 0.5f;
		}
		cur_build_node.append_attribute("pos_x") = pos.x;
		cur_build_node.append_attribute("pos_y") = pos.y;

		//Save building life
		cur_build_node.append_attribute("life") = current_building._Ptr->_Myval->GetLife();

		//Save building diplomacy
		cur_build_node.append_attribute("diplomacy") = current_building._Ptr->_Myval->GetDiplomacy();

		//Save current building selected
		cur_build_node.append_attribute("selected") = current_building._Ptr->_Myval->GetIfSelected();

		//Save current building tasks -----

		//Node where entity actions are saved
		pugi::xml_node actions_node = cur_build_node.append_child("actions");

		//Get entity worker
		ActionWorker* worker = current_building._Ptr->_Myval->GetWorker();

		//Node where primary actions data area saved
		pugi::xml_node primary_actions_node = actions_node.append_child("primary");

		//Get worker current primary action
		std::list<Action*> primary_list = worker->GetActionList(TASK_CHANNELS::PRIMARY);

		//Iterate primary actions & save them
		std::list<Action*>::const_iterator cur_primary_act = primary_list.begin();
		while (cur_primary_act != primary_list.end())
		{
			//Node where current primary action is saved
			pugi::xml_node cur_primary_act_node = primary_actions_node.append_child("action");

			//Save the current primary action in the node
			App->action_manager->SaveTask(cur_primary_act_node, cur_primary_act._Ptr->_Myval);

			cur_primary_act++;
		}

		// ----------------------------

		//Focus the next building
		current_building++;
	}
	// ------------------------------------------

	// Units Save -------------------------------
	//Units node
	pugi::xml_node units_node = data.append_child("units");

	//Iterate all the current units
	std::list<Unit*>::const_iterator current_unit = units.begin();
	while (current_unit != units.end())
	{
		/*
		- UNIT_TYPE
		- diplomacy
		- life
		- position
		*/

		//Node where the current unit is saved
		pugi::xml_node cur_unit_node = units_node.append_child("unit");


		//Save current unit type
		UNIT_TYPE unit_type = current_unit._Ptr->_Myval->GetUnitType();
		cur_unit_node.append_attribute("unit_type") = unit_type;

		//Save current unit diplomacy
		cur_unit_node.append_attribute("diplomacy") = current_unit._Ptr->_Myval->GetDiplomacy();

		//Save current unit life
		cur_unit_node.append_attribute("life") = current_unit._Ptr->_Myval->GetLife();

		//Save current unit position
		cur_unit_node.append_attribute("pos_x") = current_unit._Ptr->_Myval->GetPosition().x;
		cur_unit_node.append_attribute("pos_y") = current_unit._Ptr->_Myval->GetPosition().y;

		//Save current unit selected
		cur_unit_node.append_attribute("selected") = current_unit._Ptr->_Myval->GetIfSelected();

		switch (unit_type)
		{
		case VILLAGER:
			/*
			- resource_collected_type
			- current_resources
			*/

			//Save collected resource type
			cur_unit_node.append_attribute("res_collected_type") = ((Villager*)current_unit._Ptr->_Myval)->GetResourceCollectedType();

			//Save current resources count
			cur_unit_node.append_attribute("current_res") = ((Villager*)current_unit._Ptr->_Myval)->GetCurrentResources();

			break;
		case WARRIOR_CHMP:
		case ARCHER_CHMP:
		case WIZARD_CHMP:
			/*
			- level
			- ability [3]
			- actived[3];
			- ability_lvl_2_current_time
			- ability_lvl_3_current_time
			*/
			//Save champion level
			cur_unit_node.append_attribute("lvl") = ((Champion*)current_unit._Ptr->_Myval)->GetLevel();

			//Save champion abilities choosed state
			cur_unit_node.append_attribute("ab_choosed_1") = ((Champion*)current_unit._Ptr->_Myval)->GetSkillChoosed(0);
			cur_unit_node.append_attribute("ab_choosed_2") = ((Champion*)current_unit._Ptr->_Myval)->GetSkillChoosed(1);
			cur_unit_node.append_attribute("ab_choosed_3") = ((Champion*)current_unit._Ptr->_Myval)->GetSkillChoosed(2);

			//Save champion abilities chosen
			cur_unit_node.append_attribute("ability_1") = ((Champion*)current_unit._Ptr->_Myval)->GetAbilityChosen(0);
			cur_unit_node.append_attribute("ability_2") = ((Champion*)current_unit._Ptr->_Myval)->GetAbilityChosen(1);
			cur_unit_node.append_attribute("ability_3") = ((Champion*)current_unit._Ptr->_Myval)->GetAbilityChosen(2);

			break;
		}

		//Save current unit tasks -----

		//Node where entity actions are saved
		pugi::xml_node actions_node = cur_unit_node.append_child("actions");

		//Get entity worker
		ActionWorker* worker = current_unit._Ptr->_Myval->GetWorker();

		//Node where primary actions data area saved
		pugi::xml_node primary_actions_node = actions_node.append_child("primary");

		//Get worker current primary action
		std::list<Action*> primary_list = worker->GetActionList(TASK_CHANNELS::PRIMARY);

		//Iterate primary actions & save them
		std::list<Action*>::const_iterator cur_primary_act = primary_list.begin();
		while (cur_primary_act != primary_list.end())
		{
			//Node where current primary action is saved
			pugi::xml_node cur_primary_act_node = primary_actions_node.append_child("action");

			//Save the current primary action in the node
			App->action_manager->SaveTask(cur_primary_act_node, cur_primary_act._Ptr->_Myval);

			cur_primary_act++;
		}


		// ----------------------------

		//Focus the next unit
		current_unit++;
	}

	// ------------------------------------------
	return true;
}

void j1EntitiesManager::AddArrow(Arrow * get)
{
	all_arrows.push_back(get);
}

void j1EntitiesManager::RemoveArrow(Arrow * this_arrow)
{
	all_arrows.remove(this_arrow);
}

//Methods to add entities definitions
bool j1EntitiesManager::AddUnitDefinition(const pugi::xml_node* unit_node, DIPLOMACY diplomacy_def)
{
	if (unit_node == nullptr)return false;

	//Generate a new unit definition from the node & check if is a special unit to allocate extra stats
	Unit* new_def = nullptr;
	Unit* new_def_enemy = nullptr;

	UNIT_TYPE unit_type = App->animator->StrToUnitEnum(unit_node->attribute("unit_type").as_string());
	if (unit_type == VILLAGER)
	{
		Villager* new_villager = new Villager();
		new_def = new_villager;
	}
	else if (unit_type == WARRIOR_CHMP)
	{
		Warrior* new_warrior = new Warrior();
		new_def = new_warrior;
	}
	else if (unit_type == ARCHER_CHMP)
	{
		Hunter* new_hunter = new Hunter();
		new_def = new_hunter;
	}
	else if (unit_type == WIZARD_CHMP)
	{

		Wizard* new_wizard = new Wizard();
		new_def = new_wizard;
	}
	else
	{
		new_def = new Unit();
	}

	//Unit ID ---------------
	/*Name*/			new_def->SetName(unit_node->attribute("name").as_string());
	/*Entity Type*/		new_def->SetEntityType(UNIT);
	/*Unit Type*/		new_def->SetUnitType(unit_type);
	/*Unit Class*/		new_def->SetUnitClass(App->animator->StrToUnitClassEnum(unit_node->attribute("unit_class").as_string()));
	/*Attack Type*/		new_def->SetAttackType(App->animator->StrToAttackEnum(unit_node->attribute("attack_type").as_string()));
	//Unit Primitives -------
	/*Vision*/			Circle vision;
	/*Vision Radius*/	vision.SetRad(unit_node->attribute("vision_rad").as_uint());
	/*Vision Color*/	vision.SetColor({ 0,255,255,255 });
						new_def->SetVision(vision);
	/*Mark*/			Circle mark;
	/*Mark Radius*/		mark.SetRad(unit_node->attribute("mark_rad").as_uint());
	/*Mark Color*/		mark.SetColor({ 255,255,255,255 });
						new_def->SetMark(mark);
	/*Soft.C*/			Circle soft_collider;
	/*Soft.C Radius*/	soft_collider.SetRad(unit_node->attribute("soft_rad").as_uint());
	/*Soft.C Color*/	soft_collider.SetColor({ 255,0,255,255 });
						new_def->SetSoftCollider(soft_collider);
	/*Hard.C*/			Circle hard_collider;
	/*Hard.C Radius*/	hard_collider.SetRad(unit_node->attribute("hard_rad").as_uint());
	/*Hard.C Color*/	hard_collider.SetColor({ 255,0,0,255 });
						new_def->SetHardCollider(hard_collider);
	/*Selection Rect*/	SDL_Rect selection_rect;
	/*S.Rect X*/		selection_rect.x = unit_node->attribute("selection_x").as_int();
	/*S.Rect Y*/		selection_rect.y = unit_node->attribute("selection_y").as_int();
	/*S.Rect W*/		selection_rect.w = unit_node->attribute("selection_w").as_int();
	/*S.Rect H*/		selection_rect.h = unit_node->attribute("selection_h").as_int();
						new_def->SetSelectionRect(selection_rect);
	/*Icon Rect*/		SDL_Rect icon_rect;
	/*I.Rect X*/		icon_rect.x = unit_node->attribute("icon_x").as_int();
	/*I.Rect Y*/		icon_rect.y = unit_node->attribute("icon_y").as_int();
	/*I.Rect W*/		icon_rect.w = unit_node->attribute("icon_w").as_int();
	/*I.Rect H*/		icon_rect.h = unit_node->attribute("icon_h").as_int();
						new_def->SetIcon(icon_rect);

	//Unit Metrics ----------
	/*Max Life*/		new_def->SetMaxLife(unit_node->attribute("max_life").as_uint());
	/*Life*/			new_def->SetLife(new_def->GetMaxLife());
	/*Life Pos*/		new_def->SetLifeBarPosition(unit_node->attribute("life_pos_x").as_int(0), unit_node->attribute("life_pos_y").as_int(0));
	/*Speed*/			new_def->SetSpeed(unit_node->attribute("speed").as_float());
	/*Attack Points*/	new_def->SetAttackHitPoints(unit_node->attribute("attack_hitpoints").as_uint());
	/*Attack Rate*/		new_def->SetAttackRate(unit_node->attribute("attack_rate").as_uint());
	/*Attack Area*/		Circle area({ 0,0 }, unit_node->attribute("attack_range").as_uint(), { 0,0 });
						area.SetColor({ 0, 0, 255, 255 });
						new_def->SetAttackArea(area);
	/*Defense*/			new_def->SetDefense(unit_node->attribute("defense").as_uint());
	/*Food Cost*/		new_def->SetFoodCost(unit_node->attribute("food_cost").as_uint());
	/*Wood Cost*/		new_def->SetWoodCost(unit_node->attribute("wood_cost").as_uint());
	/*Gold Cost*/		new_def->SetGoldCost(unit_node->attribute("gold_cost").as_uint());
	/*Population Cost*/	new_def->SetPopulationCost(unit_node->attribute("population_cost").as_uint());
	/*Train Time*/		new_def->SetTrainTime(unit_node->attribute("train_time").as_uint());
	/*Unit Experience*/	new_def->SetUnitExperience(unit_node->attribute("unit_experience").as_uint());

	//Fill the extra allocated special unit stats
	bool chmp = false;
	if (unit_type == VILLAGER)
	{
		/*Resources Capacity*/		((Villager*)new_def)->SetResourcesCapacity(unit_node->attribute("resources_capacity").as_uint());
		/*Recollect Capacity*/		((Villager*)new_def)->SetRecollectCapacity(unit_node->attribute("recollect_capacity").as_uint());
		/*Stone Recollect Rate*/	((Villager*)new_def)->SetRecollectRate(unit_node->attribute("stone_recollect_rate").as_uint(),RESOURCE_TYPE::STONE_ORE);
		/*Gold Recollect Rate*/		((Villager*)new_def)->SetRecollectRate(unit_node->attribute("gold_recollect_rate").as_uint(), RESOURCE_TYPE::GOLD_ORE);
		/*Tree Recollect Rate*/		((Villager*)new_def)->SetRecollectRate(unit_node->attribute("tree_recollect_rate").as_uint(), RESOURCE_TYPE::TREE);
		/*Bush Recollect Rate*/		((Villager*)new_def)->SetRecollectRate(unit_node->attribute("bush_recollect_rate").as_uint(), RESOURCE_TYPE::BERRY_BUSH);
	}
	else if (unit_type == WARRIOR_CHMP)
	{
		/*Attakc Triangle*/			Triangle atk_triangle;
		/*Atk Triangle Length*/		atk_triangle.SetLength(unit_node->attribute("atk_triangle_length").as_uint());
		/*Atk Triangle Width*/		atk_triangle.SetWidthAngle(unit_node->attribute("atk_triangle_width_angle").as_float());
									atk_triangle.SetXAngle(12.0);
									((Warrior*)new_def)->SetSpecialAttackArea(atk_triangle);
									Circle circle;
									circle.SetColor({ 100, 100, 100, 255 });
		/*Ability lvl 3 area*/		circle.SetRad(unit_node->attribute("area_ability_lvl_3").as_uint());
									((Warrior*)new_def)->SetAreaAbilityLvl3(circle);
		/*Ability lvl 2 Atk Val*/	((Warrior*)new_def)->SetAbility_lvl_2_AttackValue(unit_node->attribute("ability_lvl_2_attack_value").as_uint());
		/*Ability lvl 2 Stun Val*/	((Warrior*)new_def)->SetAbility_lvl_2_StunValue(unit_node->attribute("ability_lvl_2_stun_value").as_uint());
		chmp = true;
	}
	else if (unit_type == ARCHER_CHMP)
	{
		/*Attakc Triangle*/			Triangle atk_triangle;
		/*Atk Triangle Length*/		atk_triangle.SetLength(unit_node->attribute("area_triangle_length").as_uint());
		/*Atk Triangle Width*/		atk_triangle.SetWidthAngle(unit_node->attribute("area_triangle_width_angle").as_float());
									atk_triangle.SetXAngle(12.0);
									atk_triangle.SetColor({ 0, 0, 0, 255 });
									((Hunter*)new_def)->SetSpecialAttackArea(atk_triangle);
		/*Circle Spell*/			Circle temp_circle;
									temp_circle.SetRad(unit_node->attribute("area_attack_skill_B_lvl_2").as_uint());
									temp_circle.SetColor({ 0,50,50,255 });
									((Hunter*)new_def)->SetSpecialAttackArea(temp_circle, "area_lvl_2");
									temp_circle.SetRad(unit_node->attribute("area_attack_skill_A_lvl_3").as_uint());
									temp_circle.SetColor({ 0,50,50,255 });
									((Hunter*)new_def)->SetSpecialAttackArea(temp_circle, "area_lvl_3");
									PivotedRect temp_rect;
									temp_rect.SetPivotDistance(unit_node->attribute("rect_pivot_distance").as_uint());
									temp_rect.SetWidth(unit_node->attribute("rect_attack_w").as_uint());
									temp_rect.SetHeight(unit_node->attribute("rect_attack_h").as_uint());
									temp_rect.SetColor({ 50,255,150,255 });
									((Hunter*)new_def)->SetSpecialAttackArea(temp_rect);
									((Hunter*)new_def)->SetAbility_lvl_2_A_AttackValue(unit_node->attribute("ability_lvl_2_skill_A_attack_value").as_uint());
									((Hunter*)new_def)->SetAbility_lvl_2_B_AttackValue(unit_node->attribute("ability_lvl_2_skill_B_attack_value").as_uint());
									((Hunter*)new_def)->SetAbility_lvl_3_A_AttackValue(unit_node->attribute("ability_lvl_3_skill_A_attack_value").as_uint());
									((Hunter*)new_def)->SetAbility_lvl_3_B_AttackValue(unit_node->attribute("ability_lvl_3_skill_B_attack_value").as_uint());
									((Hunter*)new_def)->SetAreaLimitLvl2(unit_node->attribute("area_limit_skill_B_lvl_2").as_uint());
									((Hunter*)new_def)->SetAreaLimitLvl3(unit_node->attribute("area_limit_skill_A_lvl_3").as_uint());

		chmp = true;

	}
	else if (unit_type == WIZARD_CHMP)
	{
		/*Circle Spell*/			Circle temp_circle;
									int limit = 0;
		/*limit Spell_2*/			limit = unit_node->attribute("area_limit_spell_2").as_int();
									((Wizard*)new_def)->SetAreaLimitLvl2(limit);
		/*area Spell_2*/			temp_circle.SetRad(unit_node->attribute("area_attack_spell_2").as_uint());
									temp_circle.SetColor({ 0,50,50,255 });
									((Wizard*)new_def)->SetSpecialAttackArea(temp_circle, "area_lvl_2");
		/*limit Spell_3*/			limit = unit_node->attribute("area_limit_spell_3").as_int();
									((Wizard*)new_def)->SetAreaLimitLvl3(limit);
		/*area Spell_3*/			temp_circle.SetRad(unit_node->attribute("area_attack_spell_3").as_uint());
									temp_circle.SetColor({ 0,50,50,255 });
									((Wizard*)new_def)->SetSpecialAttackArea(temp_circle, "area_lvl_3");
									((Wizard*)new_def)->SetAbility_lvl_3_AttackValue(unit_node->attribute("ability_lvl_3_attack_value").as_int());
									((Wizard*)new_def)->SetAbility_lvl_2_HealValue(unit_node->attribute("ability_lvl_2_heal_value").as_int());

		chmp = true;

	}

	if (chmp)
	{
		/*Buff Area*/				Circle buff_area({ 0,0 }, unit_node->attribute("buff_area_rad").as_uint());
									buff_area.SetColor({ 255,50,50,255 });
									((Champion*)new_def)->SetBuffArea(buff_area);
		/*level 2 cap*/				((Champion*)new_def)->SetAttackForLevel(unit_node->attribute("experience_lvl_2").as_uint());
		/*level 3 cap*/				((Champion*)new_def)->SetAttackForLevel(unit_node->attribute("experience_lvl_3").as_uint());
		/*Ability lvl 3 Cooldown*/	((Champion*)new_def)->SetAbility_lvl_2_Cooldown(unit_node->attribute("ability_lvl_2_cooldown").as_uint());
		/*Ability lvl 2 Cooldown*/	((Champion*)new_def)->SetAbility_lvl_3_Cooldown(unit_node->attribute("ability_lvl_3_cooldown").as_uint());
		/*Attack for level*/		((Champion*)new_def)->SetAttackForLevel(unit_node->attribute("attack_for_level").as_uint());
		/*Range for level*/			((Champion*)new_def)->SetRangeForLevel(unit_node->attribute("range_for_level").as_uint());
		/*Life for level*/			((Champion*)new_def)->SetRangeForLevel(unit_node->attribute("life_for_level").as_uint());
		/*Defense for level*/		((Champion*)new_def)->SetDefenseForLevel(unit_node->attribute("defense_for_level").as_float());
		/*Speed for level*/			((Champion*)new_def)->SetSpeedForLevel(unit_node->attribute("speed_for_level").as_float());
		/*View Area for level*/		((Champion*)new_def)->SetViewAreaForLevel(unit_node->attribute("view_area_for_level").as_uint());

	}

	//Add the generated unit in the units definitions entities manager array
	if(diplomacy_def == ALLY)ally_units_defs.push_back(new_def);
	else if(diplomacy_def == ENEMY)enemy_units_defs.push_back(new_def);
	else
	{
		switch (new_def->GetUnitType())
		{
		case WARRIOR_CHMP:
			new_def_enemy = new Warrior(*(Warrior*)new_def);
			break;
		case ARCHER_CHMP:
			new_def_enemy = new Hunter(*(Hunter*)new_def);
			break;
		case WIZARD_CHMP:
			new_def_enemy = new Wizard(*(Wizard*)new_def);
			break;
		case VILLAGER:
			new_def_enemy = new Villager(*(Villager*)new_def);
			break;
		default:
			new_def_enemy = new Unit(*(Unit*)new_def);
			break;
		}
		enemy_units_defs.push_back(new_def_enemy);
		ally_units_defs.push_back(new_def);
	}

	LOG("%s definition built!", new_def->GetName());

	return true;
}

bool j1EntitiesManager::AddResourceDefinition(const pugi::xml_node* resource_node)
{
	if (resource_node == nullptr)return false;

	//Get resource type
	RESOURCE_TYPE res_type = App->animator->StrToResourceEnum(resource_node->attribute("resource_type").as_string());

	//Generate a new resource definition from the node
	Resource* new_def = nullptr;
	if(res_type != TREE)new_def = new Resource();
	else  new_def = new Tree();

	//Resource ID -----------
	/*Name*/			new_def->SetName(resource_node->attribute("name").as_string());
	/*Entity Type*/		new_def->SetEntityType(RESOURCE);
	/*Resource Type*/	new_def->SetResourceType(res_type);

	//Resource Primitives ---
	/*Mark*/			Rectng mark;
	/*Mark Width*/		mark.SetWidth(resource_node->attribute("mark_w").as_uint());
	/*Mark Height*/		mark.SetHeight(mark.GetWidth() * sin(mark.GetXAngle()));
	/*Mark Color*/		mark.SetColor({ 255,255,255,255 });
						new_def->SetMark(mark);
	/*Interaction Area*/Circle area({ 0,0 }, resource_node->attribute("interaction_rad").as_uint(), { 0,0 });
						area.SetColor({ 0, 0, 255, 255 });
						new_def->SetInteractArea(area);
	/*Selection Rect*/	SDL_Rect selection_rect;
	/*S.Rect X*/		selection_rect.x = resource_node->attribute("selection_x").as_int();
	/*S.Rect Y*/		selection_rect.y = resource_node->attribute("selection_y").as_int();
	/*S.Rect W*/		selection_rect.w = resource_node->attribute("selection_w").as_int();
	/*S.Rect H*/		selection_rect.h = resource_node->attribute("selection_h").as_int();
						new_def->SetSelectionRect(selection_rect);
	/*Icon Rect*/		SDL_Rect icon_rect;
	/*I.Rect X*/		icon_rect.x = resource_node->attribute("icon_x").as_int();
	/*I.Rect Y*/		icon_rect.y = resource_node->attribute("icon_y").as_int();
	/*I.Rect W*/		icon_rect.w = resource_node->attribute("icon_w").as_int();
	/*I.Rect H*/		icon_rect.h = resource_node->attribute("icon_h").as_int();
						new_def->SetIcon(icon_rect);

	//Resource Metrics ------
	/*Max Resources*/	new_def->SetMaxLife(resource_node->attribute("max_resources").as_uint());
	/*C.Resources*/		new_def->SetLife(new_def->GetMaxLife());

	if(res_type == TREE)
	{
		/*Tree Cortex*/	((Tree*)new_def)->SetCortex(resource_node->attribute("cortex").as_uint());
	}
	resources_defs.push_back(new_def);

	LOG("%s definition built!", new_def->GetName());

	return true;
}

bool j1EntitiesManager::AddBuildingDefinition(const pugi::xml_node * building_node, DIPLOMACY diplomacy_def)
{
	if (building_node == nullptr)return false;

	//Get building type to allocate the necessary attributes
	BUILDING_TYPE building_type = App->animator->StrToBuildingEnum(building_node->attribute("building_type").as_string());

	//Generate a new building definition from the node
	ProductiveBuilding* new_def = new ProductiveBuilding();

	//Building ID -----------
	/*Name*/			new_def->SetName(building_node->attribute("name").as_string());
	/*Entity Type*/		new_def->SetEntityType(BUILDING);
	/*Building Type*/	new_def->SetBuildingType(building_type);

	//Building Primitives ---
	/*Vision*/			Circle vision;
	/*Vision Radius*/	vision.SetRad(building_node->attribute("vision_rad").as_uint());
	/*Vision Color*/	vision.SetColor({ 0,255,255,255 });
						new_def->SetVision(vision);
	/*Mark*/			Rectng mark;
	/*Mark Width*/		mark.SetWidth(building_node->attribute("mark_w").as_uint());
	/*Mark Height*/		mark.SetHeight(mark.GetWidth() * sin(mark.GetXAngle()));
	/*Mark Displace*/	iPoint displacement(building_node->attribute("mark_x").as_int(), building_node->attribute("mark_y").as_int());
						mark.SetDisplacement(displacement);
	/*Mark Color*/		mark.SetColor({ 55,255,255,255 });
						new_def->SetMark(mark);
	/*Interaction Area*/Rectng area;
	/*I.Area Width*/	area.SetWidth(building_node->attribute("interaction_area_w").as_uint());
	/*I.Area Height*/	area.SetHeight(area.GetWidth() * sin(area.GetXAngle()));
	/*I.Area Displace*/	displacement.create(building_node->attribute("interaction_area_x").as_int(), building_node->attribute("interaction_area_y").as_int());
						area.SetDisplacement(displacement);
	/*I.Area Color*/	area.SetColor({ 0,0,255,255 });
						new_def->SetInteractArea(area);
	/*W. in Tiles*/		new_def->SetWidthInTiles(building_node->attribute("width_in_tiles").as_uint());
	/*H. in Tiles*/		new_def->SetHeightInTiles(building_node->attribute("height_in_tiles").as_uint());
	/*Selection Rect*/	SDL_Rect selection_rect;
	/*S.Rect X*/		selection_rect.x = building_node->attribute("selection_x").as_int();
	/*S.Rect Y*/		selection_rect.y = building_node->attribute("selection_y").as_int();
	/*S.Rect W*/		selection_rect.w = building_node->attribute("selection_w").as_int();
	/*S.Rect H*/		selection_rect.h = building_node->attribute("selection_h").as_int();
						new_def->SetSelectionRect(selection_rect);
	/*Icon Rect*/		SDL_Rect icon_rect;
	/*I.Rect X*/		icon_rect.x = building_node->attribute("icon_x").as_int();
	/*I.Rect Y*/		icon_rect.y = building_node->attribute("icon_y").as_int();
	/*I.Rect W*/		icon_rect.w = building_node->attribute("icon_w").as_int();
	/*I.Rect H*/		icon_rect.h = building_node->attribute("icon_h").as_int();
						new_def->SetIcon(icon_rect);

	//Building Stats --------
	/*Max Life*/		new_def->SetMaxLife(building_node->attribute("max_life").as_uint());
						new_def->SetLife(new_def->GetMaxLife());
	/*Life Pos*/		new_def->SetLifeBarPosition(building_node->attribute("life_pos_x").as_int(0), building_node->attribute("life_pos_y").as_int(0));


	/*Units Capacity*/	((ProductiveBuilding*)new_def)->SetUnitsCapacity(building_node->attribute("units_capacity").as_uint());
	/*Units Spawn pnt*/	iPoint spawn(building_node->attribute("units_spawn_x").as_int(), building_node->attribute("units_spawn_y").as_int());
						((ProductiveBuilding*)new_def)->SetUnitsSpawnPoint(spawn);
	/*Production Cap*/	((ProductiveBuilding*)new_def)->SetProductionCapacity(building_node->attribute("production_capacity").as_uint());


	if(diplomacy_def == ALLY)ally_buildings_defs.push_back(new_def);
	else if(diplomacy_def == ENEMY)enemy_buildings_defs.push_back(new_def);
	else
	{
		ProductiveBuilding* enemy_new_def = new ProductiveBuilding(*new_def);
		enemy_buildings_defs.push_back(enemy_new_def);
		ally_buildings_defs.push_back(new_def);
	}

	LOG("%s definition built!", new_def->GetName());

	return true;
}

//Check if the entity civilizations string contains the chosen one
bool j1EntitiesManager::CivilizationCheck(char * civs_str, const char * chosen_civ)
{
	if (civs_str == nullptr || chosen_civ == nullptr)return false;

	char* civ = strtok(civs_str, "/");
	while (civ != NULL)
	{

		if (strcmp(civ, chosen_civ) == 0)return true;
		civ = strtok(NULL, "/");

	}
	return false;
}

void j1EntitiesManager::GetChampion(Champion * champion, DIPLOMACY diplomacy)
{
	if (diplomacy == ENEMY)
		champions_red.push_back(champion);
	else
		champions_blue.push_back(champion);

}

void j1EntitiesManager::ExtractChampion(Champion * champion, DIPLOMACY diplomacy)
{
	if (diplomacy == ENEMY)
		champions_red.remove(champion);
	else
		champions_blue.remove(champion);
}

void j1EntitiesManager::GetExperienceFromUnit(int exp, DIPLOMACY diplomacy)
{
	if (diplomacy == ENEMY || diplomacy == NEUTRAL) {
		if (!champions_blue.empty()) {
			std::list<Champion*>::const_iterator champion = champions_blue.begin();
			Champion* current_chmp = nullptr;
			int no_maxed_chmp = 0;
			while (champion != champions_blue.end())
			{
				current_chmp = champion._Ptr->_Myval;
				if (current_chmp->GetLevel() != MAX_LEVEL)	no_maxed_chmp++;
				champion++;
			}
			//check if champions need exp;
			if (no_maxed_chmp == 0)	return;

			champion = champions_blue.begin();
			int exp_partition = exp / no_maxed_chmp;
			while (champion != champions_blue.end())
			{
				current_chmp = champion._Ptr->_Myval;
				if (current_chmp->GetLevel() != MAX_LEVEL)	current_chmp->SetExperience(exp_partition);
				champion++;

			}
		}
	}
	if (diplomacy == ALLY || diplomacy == NEUTRAL) {
		if (!champions_red.empty()) {
			std::list<Champion*>::const_iterator champion = champions_red.begin();
			Champion* current_chmp = nullptr;
			int no_maxed_chmp = 0;
			while (champion != champions_red.end())
			{
				current_chmp = champion._Ptr->_Myval;
				if (current_chmp->GetLevel() != MAX_LEVEL)	no_maxed_chmp++;
				champion++;

			}
			//check if champions need exp;
			if (no_maxed_chmp == 0)	return;

			champion = champions_red.begin();
			int exp_partition = exp / no_maxed_chmp;
			while (champion != champions_red.end())
			{
				current_chmp = champion._Ptr->_Myval;
				if (current_chmp->GetLevel() != MAX_LEVEL)	current_chmp->SetExperience(exp_partition);
				champion++;
			}
		}
	}
}

//Functionality =======================
Unit* j1EntitiesManager::GenerateUnit(UNIT_TYPE type, DIPLOMACY diplomacy, bool push_in_list)
{
	Unit* new_unit = nullptr;

	uint def_num = ally_units_defs.size();
	for (uint k = 0; k < def_num; k++)
	{
		if (ally_units_defs[k]->GetUnitType() == type)
		{
			//If the unit to generate is a special unit we need to allocate extra stats
			if (type == VILLAGER)
			{
				Villager* new_villager;
				if (diplomacy == ALLY)new_villager = new Villager(*(Villager*)ally_units_defs[k]);
				else new_villager = new Villager(*(Villager*)enemy_units_defs[k]);

				new_unit = new_villager;
			}
			else if (type == WARRIOR_CHMP)
			{
				Warrior* new_warrior;
				if (diplomacy == ALLY)
					new_warrior = new Warrior(*(Warrior*)ally_units_defs[k]);
				else new_warrior = new Warrior(*(Warrior*)enemy_units_defs[k]);

				GetChampion(new_warrior, diplomacy);
				new_unit = new_warrior;
			}
			else if (type == ARCHER_CHMP)
			{
				Hunter* new_hunter;
				if (diplomacy == ALLY)new_hunter = new Hunter(*(Hunter*)ally_units_defs[k]);
				else new_hunter = new Hunter(*(Hunter*)enemy_units_defs[k]);

				GetChampion(new_hunter, diplomacy);
				new_unit = new_hunter;
			}
			else if (type == WIZARD_CHMP)
			{
				Wizard* new_wizard;
				if (diplomacy == ALLY)new_wizard = new Wizard(*(Wizard*)ally_units_defs[k]);
				else new_wizard = new Wizard(*(Wizard*)enemy_units_defs[k]);

				GetChampion(new_wizard, diplomacy);
				new_unit = new_wizard;
			}
			else
			{
				//Build unit
				if (diplomacy == ALLY)new_unit = new Unit(*ally_units_defs[k]);
				else new_unit = new Unit(*enemy_units_defs[k]);
			}

			//Set generated unit diplomacy
			new_unit->SetDiplomacy(diplomacy);

			//Set generated unit render area with the vision area specified
			Circle rend = new_unit->GetVision();
			rend.SetRad(rend.GetRad() + RENDER_MARGIN);
			new_unit->SetRenderArea(rend);

			//Set unit animation
			App->animator->UnitPlay(new_unit);

			//Add the new unit at the units manage list
			if (push_in_list)
			{
				units.push_back(new_unit);
			}

			return new_unit;
		}
	}
	return nullptr;
}

Building* j1EntitiesManager::GenerateBuilding(BUILDING_TYPE type, DIPLOMACY diplomacy, bool push_in_list)
{
	Building* new_building = nullptr;

	uint def_num = ally_buildings_defs.size();
	for (uint k = 0; k < def_num; k++)
	{
		if (ally_buildings_defs[k]->GetBuildingType() == type)
		{
			if(diplomacy == ALLY)new_building = new ProductiveBuilding(*(ProductiveBuilding*)ally_buildings_defs[k]);
			else new_building = new ProductiveBuilding(*(ProductiveBuilding*)enemy_buildings_defs[k]);

			//Set unit animation
			App->animator->BuildingPlay(new_building);

			//Set unit diplomacy
			new_building->SetDiplomacy(diplomacy);

			//Set generated building render area with the vision area specified
			Circle rend = new_building->GetVision();
			rend.SetRad(rend.GetRad() + RENDER_MARGIN);
			new_building->SetRenderArea(rend);

			//If the building is an ally fog is already discovered
			if (diplomacy == ALLY)new_building->SetFogDiscovered(true);

			if (push_in_list)
			{
				//Add the new building at the buildings manage list
				buildings.push_back(new_building);

			}

			return new_building;
		}
	}
	return nullptr;
}

Resource* j1EntitiesManager::GenerateResource(RESOURCE_TYPE type)
{
	Resource* new_resource = nullptr;

	uint def_num = resources_defs.size();
	for (uint k = 0; k < def_num; k++)
	{
		if (resources_defs[k]->GetResourceType() == type)
		{
			//Build unit
			if(type != TREE)new_resource = new Resource(*resources_defs[k]);
			else new_resource = new Tree(*(Tree*)resources_defs[k]);

			//Set unit animation
			App->animator->ResourcePlay(new_resource);

			//Add the new unit at the units manage list
			resources.push_back(new_resource);

			return new_resource;
		}
	}
	return nullptr;
}

const std::list<Unit*>* j1EntitiesManager::UnitsList() const
{
	return &units;
}

const std::list<Building*>* j1EntitiesManager::BuildingList() const
{
	return &buildings;
}

const std::list<Resource*>* j1EntitiesManager::ResourceList() const
{
	return &resources;
}

bool j1EntitiesManager::DeleteEntity(Entity * entity)
{
	//Check if the entity is really defined
	if (entity == nullptr)
	{
		return false;
	}

	ready_to_waste.remove(entity);
	ready_to_waste.push_back(entity);

	return true;
}

bool j1EntitiesManager::SetUnitPath(Unit* target, const iPoint& goal)
{
	fPoint target_pos = target->GetPosition().Round();
	iPoint int_target_pos(target_pos.x, target_pos.y);

	//Check if the position and the goal are the same
	if (goal == int_target_pos)return false;

	//Create path from unit position to the goal
	std::vector<iPoint>* path = App->pathfinding->SimpleAstar(int_target_pos, goal);

	if (path == nullptr)return false;

	//Set target path
	if (target->GetAction() != WALK)
	{
		target->SetAction(WALK);
		target->Focus(path->front());
	}

	//Play move fx


	return true;
}

bool j1EntitiesManager::SetGroupPath(const std::vector<Unit*>& targets, const iPoint& goal)
{
	/*
	if(targets.size() > 0)return false;

	uint size = targets.size();
	for (uint k = 0; k < size; k++)
	{
	//Get target position
	fPoint target_pos = targets[k]->GetPosition();
	//Calculate path from target position to goal
	std::vector<iPoint>* path = App->pathfinding->SimpleAstar(iPoint(target_pos.x, target_pos.y), goal);
	//Return false if the path is incorrect
	if (path == nullptr)return false;
	//Set current target path
	targets[k]->SetPath(path);
	}
	*/
	return true;
}

std::list<Unit*> j1EntitiesManager::GetDeathUnitList()
{
	return death_units;
}

void j1EntitiesManager::AddUnit(Unit* unit)
{
	units.push_back((Unit*)unit);
	units_quadtree.Exteract(unit, &unit->GetPosition());
	units_quadtree.Insert((Unit*)unit, &unit->GetPosition());
}

void j1EntitiesManager::AddDeathUnit(Unit * unit)
{

	death_units.push_back((Unit*)unit);
	units_quadtree.Exteract(unit, &unit->GetPosition());

}

void j1EntitiesManager::ResurrectUnit(Unit * unit)
{
	units.push_back((Unit*)unit);
	death_units.remove(unit);
	units_quadtree.Insert(unit, &unit->GetPosition());
}


void j1EntitiesManager::RemoveDeathUnit(Unit * unit)
{
	death_units.remove(unit);
}

Unit * j1EntitiesManager::PopUnit(const Unit * unit)
{
	units.remove((Unit*)unit);
	return (Unit*)unit;
}

void j1EntitiesManager::AddDeadBuilding(Building * build)
{
	death_buildings.push_back(build);
	buildings_quadtree.Exteract(build, &build->GetPosition());
}

void j1EntitiesManager::RemoveDeathBuilding(Building * build)
{
	death_buildings.remove(build);
}

void j1EntitiesManager::AddDeadResource(Resource * res)
{
	death_resources.push_back(res);
}

void j1EntitiesManager::RemoveDeathResource(Resource * res)
{
	death_resources.remove(res);
}

std::priority_queue<Unit*, std::vector<Unit*>, LessDistance > j1EntitiesManager::OrganizeByNearest(std::vector<Unit*>& vec, Circle& target)
{
	std::priority_queue<Unit*, std::vector<Unit*>, LessDistance  > organized;
	uint size = vec.size();
	iPoint pos = target.GetPosition();
	fPoint perf_pos(pos.x, pos.y);

	for (uint k = 0; k < size; k++)
	{
		fPoint dist = vec[k]->GetPosition();
		dist.x -= pos.x;
		dist.y -= pos.y;
		float perf_dist = sqrt(dist.x*dist.x + dist.y * dist.y);
		vec[k]->distance_to_target = perf_dist;
		organized.push(vec[k]);
	}

	vec.clear();

	while (!organized.empty())
	{
		Unit* unit = organized.top();
		organized.pop();
		vec.push_back(unit);
	}



	return organized;
}

void j1EntitiesManager::UpgradeEntity(RESEARCH_TECH type, DIPLOMACY diplomacy)
{
	// Upgrade Entity acording to research tecnology type
	switch (type)
	{
	case NO_TECH:
		break;
	case A_ARCHER_UP: UpgradeUnit(ARCHER, CROSSBOWMAN, diplomacy);
		break;
	case A_C_ARCHER_UP: UpgradeUnit(CAVALRY_ARCHER, HEAVY_CAVALRY_ARCHER, diplomacy);
		break;
	case A_CROSSBOW_UP: UpgradeUnit(CROSSBOWMAN, ARBALEST, diplomacy);
		break;
	case A_SKIMISHER_UP:
		break;
	case A_THUMBRING_UP:
		break;
	case B_MILITIA_UP:		UpgradeUnit(MILITIA, MAN_AT_ARMS, diplomacy);
		break;
	case B_MAN_AT_ARMS_UP:	UpgradeUnit(MAN_AT_ARMS, LONG_SWORDMAN, diplomacy);
		break;
	case B_LONGSWORDSMAN_UP:UpgradeUnit(LONG_SWORDMAN, TWO_HANDED_SWORDMAN, diplomacy);
		break;
	case B_TWO_HANDED_UP:	UpgradeUnit(TWO_HANDED_SWORDMAN, CHAMPION, diplomacy);
		break;
	case B_SPEARMAN_UP:		UpgradeUnit(SPEARMAN, PIKEMAN, diplomacy);
		break;
	case S_KNIGHT_UP:		UpgradeUnit(KNIGHT, CAVALIER, diplomacy);
		break;
	case S_CAVALLIER_UP:	UpgradeUnit(CAVALIER, PALADIN, diplomacy);
		break;
	case S_SCOUTC_UP:
		break;
	default:

		//Researches upgrades
		UpgradeResearch(type, diplomacy);
		break;
	}
}

void j1EntitiesManager::UpgradeUnit(UNIT_TYPE u_type, UNIT_TYPE new_type, DIPLOMACY e_diplomacy)
{
	if (u_type == NO_UNIT || new_type == NO_UNIT || e_diplomacy == NEUTRAL)return;

	Unit* new_unit = nullptr;
	int size = ally_units_defs.size();
	for (int count = 0; count < size; count++)
	{
		if (ally_units_defs[count]->GetUnitType() == new_type)
		{
			if(e_diplomacy == ALLY) new_unit = ally_units_defs[count];
			else new_unit = enemy_units_defs[count];
			break;
		}
	}
	std::list<Unit*>::iterator unit = units.begin();
	while (unit != units.end())
	{
		if (unit._Ptr->_Myval->GetDiplomacy() == e_diplomacy && unit._Ptr->_Myval->GetUnitType() == u_type)
		{
			App->buff_manager->RemoveTargetBuffs(unit._Ptr->_Myval);
			unit._Ptr->_Myval->SetUpgrade(new_unit);
		}
		unit++;
	}
	
	std::list<Action*>::iterator curr_action;
	std::list<Action*>*	action_list;
	std::list<Building*>::iterator building = buildings.begin();
	while (building != buildings.end())
	{
		if (building._Ptr->_Myval->GetDiplomacy() == e_diplomacy)
		{
			action_list = building._Ptr->_Myval->GetWorker()->GetActionListPointer(PRIMARY);
			curr_action = action_list->begin();
			while (curr_action != action_list->end())
			{
				if (curr_action._Ptr->_Myval->GetType() == TASK_B_SPAWN_UNITS)
				{
					if (((SpawnUnitAction*)curr_action._Ptr->_Myval)->GetUnitType() == u_type)
						((SpawnUnitAction*)curr_action._Ptr->_Myval)->SetUnitType(new_type);
				}
				curr_action++;
			}
			if (building._Ptr->_Myval->GetWorker()->GetCurrentActionType() == TASK_B_SPAWN_UNITS)
			{
				SpawnUnitAction* spawn_action = (SpawnUnitAction*)building._Ptr->_Myval->GetWorker()->GetCurrentPrimaryAction();
				if(spawn_action->GetUnitType() == u_type)spawn_action->SetUnitType(new_type);
			}

		}
		building++;
	}
	
}

void j1EntitiesManager::UpgradeBuilding(BUILDING_TYPE b_type, BUILDING_TYPE new_type, DIPLOMACY diplomacy)
{
	if (b_type == NO_BUILDING || new_type == NO_BUILDING || diplomacy == NEUTRAL)return;

	
	Building* new_building = nullptr;
	int size = ally_buildings_defs.size();
	for (int count = 0; count < size; count++)
	{
		if (ally_buildings_defs[count]->GetBuildingType() == new_type)
		{
			if(diplomacy == ALLY)new_building = ally_buildings_defs[count];
			else new_building = enemy_buildings_defs[count];
			break;
		}
	}
	std::list<Building*>::iterator build = buildings.begin();
	while (build != buildings.end())
	{
		if (build._Ptr->_Myval->GetDiplomacy() == diplomacy && build._Ptr->_Myval->GetBuildingType() == b_type)
		{
			build._Ptr->_Myval->SetUpgrade(new_building);
		}
		build++;
	}
}

bool j1EntitiesManager::UpgradeResearch(RESEARCH_TECH research_type, DIPLOMACY diplomacy)
{
	switch (research_type)
	{
	case BS_PADDED_AA:
	case BS_LEATHER_AA:
	case BS_RING_AA:
	{
		uint size = App->entities_manager->ally_units_defs.size();
		for (uint k = 0; k < size; k++)
		{
			if (App->entities_manager->ally_units_defs[k]->GetUnitClass() == UNIT_CLASS::ARCHERY)
			{
				Unit* target = nullptr;
				if (diplomacy == ALLY)target = App->entities_manager->ally_units_defs[k];
				else target = App->entities_manager->enemy_units_defs[k];

				target->SetDefense(target->GetDefense() + 1);
			}
		}
		std::list<Unit*>::const_iterator unit = App->entities_manager->units.begin();
		while (unit != App->entities_manager->units.end())
		{
			if (unit._Ptr->_Myval->GetUnitClass() == ARCHERY && unit._Ptr->_Myval->GetDiplomacy() == diplomacy)
			{
				unit._Ptr->_Myval->SetDefense(unit._Ptr->_Myval->GetDefense() + 1);
			}
			unit++;
		}
		break;
	}
	case BS_FLETCHING:
	case BS_BODKINARROW:
	case BS_BRACER:
	{
		uint size = App->entities_manager->ally_units_defs.size();
		for (uint k = 0; k < size; k++)
		{
			if (App->entities_manager->ally_units_defs[k]->GetUnitClass() == UNIT_CLASS::ARCHERY)
			{
				Unit* target = nullptr;
				if (diplomacy == ALLY)target = App->entities_manager->ally_units_defs[k];
				else target = App->entities_manager->enemy_units_defs[k];

				target->SetAttackHitPoints(target->GetAttackHitPoints() + 1);
			}
		}
		std::list<Unit*>::const_iterator unit = App->entities_manager->units.begin();
		while (unit != App->entities_manager->units.end())
		{
			if (unit._Ptr->_Myval->GetUnitClass() == ARCHERY && unit._Ptr->_Myval->GetDiplomacy() == diplomacy)
			{
				unit._Ptr->_Myval->SetAttackHitPoints(unit._Ptr->_Myval->GetAttackHitPoints() + 1);
			}
			unit++;
		}
		break;
	}
	case BS_FORGING:
	case BS_IRONCASTING:
	case BS_BLASTFURNACE:
	{
		uint upgrade_value = 1;
		if (research_type == BS_BLASTFURNACE)upgrade_value = 2;

		uint size = App->entities_manager->ally_units_defs.size();
		for (uint k = 0; k < size; k++)
		{
			UNIT_CLASS unit_class = App->entities_manager->ally_units_defs[k]->GetUnitClass();
			if (unit_class == ARCHERY || unit_class == INFANTRY || unit_class == CAVALRY)
			{
				Unit* target = nullptr;
				if (diplomacy == ALLY)target = App->entities_manager->ally_units_defs[k];
				else target = App->entities_manager->enemy_units_defs[k];

				target->SetAttackHitPoints(target->GetAttackHitPoints() + upgrade_value);
			}
		}
		std::list<Unit*>::const_iterator unit = App->entities_manager->units.begin();
		while (unit != App->entities_manager->units.end())
		{
			UNIT_CLASS unit_class = unit._Ptr->_Myval->GetUnitClass();
			if (unit_class == ARCHERY || unit_class == INFANTRY || unit_class == CAVALRY && unit._Ptr->_Myval->GetDiplomacy() == diplomacy)
			{
				unit._Ptr->_Myval->SetAttackHitPoints(unit._Ptr->_Myval->GetAttackHitPoints() + upgrade_value);
			}
			unit++;
		}
		break;
	}
	case BS_SCALE_BA:
	case BS_CHAIN_BA:
	case BS_PLATE_BA:
	{
		uint upgrade_value = 1;
		if (research_type == BS_PLATE_BA)upgrade_value = 2;

		uint size = App->entities_manager->ally_units_defs.size();
		for (uint k = 0; k < size; k++)
		{
			if (App->entities_manager->ally_units_defs[k]->GetUnitClass() == CAVALRY)
			{
				Unit* target = nullptr;
				if (diplomacy == ALLY)target = App->entities_manager->ally_units_defs[k];
				else target = App->entities_manager->enemy_units_defs[k];

				target->SetDefense(target->GetDefense() + upgrade_value);
			}
		}
		std::list<Unit*>::const_iterator unit = App->entities_manager->units.begin();
		while (unit != App->entities_manager->units.end())
		{
			if (unit._Ptr->_Myval->GetUnitClass() == CAVALRY && unit._Ptr->_Myval->GetDiplomacy() == diplomacy)
			{
				unit._Ptr->_Myval->SetDefense(unit._Ptr->_Myval->GetDefense() + upgrade_value);
			}
			unit++;
		}
		break;
	}
	case BS_SCALE_MAIL_ARMOR:
	case BS_CHAIN_MAIL_ARMOR:
	case BS_PLATE_MAIL_ARMOR:
	{
		uint upgrade_value = 1;
		if (research_type == BS_PLATE_MAIL_ARMOR)upgrade_value = 2;

		uint size = App->entities_manager->ally_units_defs.size();
		for (uint k = 0; k < size; k++)
		{
			if (App->entities_manager->ally_units_defs[k]->GetUnitClass() == INFANTRY)
			{
				Unit* target = nullptr;
				if (diplomacy == ALLY)target = App->entities_manager->ally_units_defs[k];
				else target = App->entities_manager->enemy_units_defs[k];

				target->SetDefense(target->GetDefense() + upgrade_value);
			}
		}
		std::list<Unit*>::const_iterator unit = App->entities_manager->units.begin();
		while (unit != App->entities_manager->units.end())
		{
			if (unit._Ptr->_Myval->GetUnitClass() == INFANTRY && unit._Ptr->_Myval->GetDiplomacy() == diplomacy)
			{
				unit._Ptr->_Myval->SetDefense(unit._Ptr->_Myval->GetDefense() + upgrade_value);
			}
			unit++;
		}
		break;
	}
	case S_BLOODLINES:
	{
		uint size = App->entities_manager->ally_units_defs.size();
		for (uint k = 0; k < size; k++)
		{
			if (App->entities_manager->ally_units_defs[k]->GetUnitClass() == CAVALRY)
			{
				Unit* target = nullptr;
				if (diplomacy == ALLY)target = App->entities_manager->ally_units_defs[k];
				else target = App->entities_manager->enemy_units_defs[k];

				target->SetMaxLife(target->GetMaxLife() + 20);
			}
		}
		std::list<Unit*>::const_iterator unit = App->entities_manager->units.begin();
		while (unit != App->entities_manager->units.end())
		{
			if (unit._Ptr->_Myval->GetUnitClass() == CAVALRY && unit._Ptr->_Myval->GetDiplomacy() == diplomacy)
			{
				unit._Ptr->_Myval->SetMaxLife(unit._Ptr->_Myval->GetMaxLife() + 20);
			}
			unit++;
		}
		break;
	}
	case S_HUSBANDRY:
	{
		uint size = App->entities_manager->ally_units_defs.size();
		for (uint k = 0; k < size; k++)
		{
			if (App->entities_manager->ally_units_defs[k]->GetUnitClass() == CAVALRY)
			{
				Unit* target = nullptr;
				if (diplomacy == ALLY)target = App->entities_manager->ally_units_defs[k];
				else target = App->entities_manager->enemy_units_defs[k];

				target->SetSpeed(target->GetSpeed() * 1.10);
			}
		}
		std::list<Unit*>::const_iterator unit = App->entities_manager->units.begin();
		while (unit != App->entities_manager->units.end())
		{
			if (unit._Ptr->_Myval->GetUnitClass() == CAVALRY && unit._Ptr->_Myval->GetDiplomacy() == diplomacy)
			{
				unit._Ptr->_Myval->SetSpeed(unit._Ptr->_Myval->GetSpeed() * 1.10);
			}
			unit++;
		}
		break;
	}
	case TC_CASTLE:
		UpgradeBuilding(TOWN_CENTER, TOWN_CENTER_C, diplomacy);
		UpgradeBuilding(BARRACK, BARRACK_C, diplomacy);
		UpgradeBuilding(ARCHERY_RANGE, ARCHERY_RANGE_C, diplomacy);
		UpgradeBuilding(BLACKSMITH, BLACKSMITH_C, diplomacy);
		UpgradeBuilding(STABLE, STABLE_C, diplomacy);
		UpgradeBuilding(HOUSE_A, HOUSE_AI, diplomacy);
		UpgradeBuilding(HOUSE_B, HOUSE_BI, diplomacy);
		UpgradeBuilding(HOUSE_C, HOUSE_CI, diplomacy);
		break;
	case TC_IMPERIAL:
		UpgradeBuilding(UNIVERSITY_C, UNIVERSITY_I, diplomacy);
		break;
	case TC_LOOM:
	{
		uint size = App->entities_manager->ally_units_defs.size();
		for (uint k = 0; k < size; k++)
		{
			if (App->entities_manager->ally_units_defs[k]->GetUnitType() == VILLAGER)
			{
				Villager* target = nullptr;
				if (diplomacy == ALLY)target = (Villager*)App->entities_manager->ally_units_defs[k];
				else target = (Villager*)App->entities_manager->enemy_units_defs[k];

				target->SetMaxLife(target->GetMaxLife() + 15);
				target->SetLife(target->GetLife() + 15);
				target->SetDefense(target->GetDefense() + 2);
				break;
			}
		}
		std::list<Unit*>::const_iterator unit = App->entities_manager->units.begin();
		while (unit != App->entities_manager->units.end())
		{
			if (unit._Ptr->_Myval->GetUnitType() == VILLAGER && unit._Ptr->_Myval->GetDiplomacy() == diplomacy)
			{
				unit._Ptr->_Myval->SetMaxLife(unit._Ptr->_Myval->GetMaxLife() + 15);
				unit._Ptr->_Myval->SetLife(unit._Ptr->_Myval->GetLife() + 15);
				unit._Ptr->_Myval->SetDefense(unit._Ptr->_Myval->GetDefense() + 2);
			}
			unit++;
		}
		break;
	}
	case TC_PATROL:
	case TC_TOWNWATCH:
	{
		uint size = App->entities_manager->ally_buildings_defs.size();
		for (uint k = 0; k < size; k++)
		{
			if (App->entities_manager->ally_buildings_defs[k]->GetBuildingType() == TOWN_CENTER)
			{
				Building* target = nullptr;
				if (diplomacy == ALLY)target = App->entities_manager->ally_buildings_defs[k];
				else App->entities_manager->enemy_buildings_defs[k];

				Circle vis = target->GetVision();
				vis.SetRad(vis.GetRad() + 50);
				target->SetVision(vis);
				Circle rend = target->GetRenderArea();
				rend.SetRad(rend.GetRad() + 50);
				target->SetRenderArea(rend);
				break;
			}
		}
		std::list<Building*>::const_iterator build = App->entities_manager->buildings.begin();
		while (build != App->entities_manager->buildings.end())
		{
			if (build._Ptr->_Myval->GetBuildingType() == TOWN_CENTER && build._Ptr->_Myval->GetDiplomacy() == diplomacy)
			{
				Circle vis = build._Ptr->_Myval->GetVision();
				vis.SetRad(vis.GetRad() + 80);
				build._Ptr->_Myval->SetVision(vis);
				Circle rend = build._Ptr->_Myval->GetRenderArea();
				rend.SetRad(rend.GetRad() + 80);
				build._Ptr->_Myval->SetRenderArea(rend);

				const fPoint pos = build._Ptr->_Myval->GetPosition();
				build._Ptr->_Myval->SetPosition(pos.x, pos.y + App->map->data.tile_height * 0.5);
			}
			build++;
		}
		break;
	}
	case TC_WHEELBARROW:
	{
		uint size = App->entities_manager->ally_units_defs.size();
		for (uint k = 0; k < size; k++)
		{
			if (App->entities_manager->ally_units_defs[k]->GetUnitType() == VILLAGER)
			{
				Villager* target = nullptr;
				if (diplomacy == ALLY)target = (Villager*)App->entities_manager->ally_units_defs[k];
				else target = (Villager*)App->entities_manager->enemy_units_defs[k];

				target->SetSpeed(target->GetSpeed() * 1.10);
				target->SetResourcesCapacity(target->GetResourcesCapacity() * 1.25);

				break;
			}
		}
		std::list<Unit*>::const_iterator unit = App->entities_manager->units.begin();
		while (unit != App->entities_manager->units.end())
		{
			if (unit._Ptr->_Myval->GetUnitType() == VILLAGER && unit._Ptr->_Myval->GetDiplomacy() == diplomacy)
			{
				unit._Ptr->_Myval->SetSpeed(unit._Ptr->_Myval->GetSpeed() * 1.10);
				((Villager*)unit._Ptr->_Myval)->SetRecollectCapacity(((Villager*)unit._Ptr->_Myval)->GetRecollectCapacity() * 1.25);
			}
			unit++;
		}
		break;
	}
	case TC_HANDCART:
	{
		uint size = App->entities_manager->ally_units_defs.size();
		for (uint k = 0; k < size; k++)
		{
			if (App->entities_manager->ally_units_defs[k]->GetUnitType() == VILLAGER)
			{
				Villager* target = nullptr;
				if (diplomacy == ALLY)target = (Villager*)App->entities_manager->ally_units_defs[k];
				else target = (Villager*)App->entities_manager->enemy_units_defs[k];

				target->SetSpeed(target->GetSpeed() * 1.10);
				target->SetResourcesCapacity(target->GetResourcesCapacity() * 1.50);

				break;
			}
		}
		std::list<Unit*>::const_iterator unit = App->entities_manager->units.begin();
		while (unit != App->entities_manager->units.end())
		{
			if (unit._Ptr->_Myval->GetUnitType() == VILLAGER && unit._Ptr->_Myval->GetDiplomacy() == diplomacy)
			{
				unit._Ptr->_Myval->SetSpeed(unit._Ptr->_Myval->GetSpeed() * 1.10);
				((Villager*)unit._Ptr->_Myval)->SetRecollectCapacity(((Villager*)unit._Ptr->_Myval)->GetRecollectCapacity() * 1.50);
			}
			unit++;
		}
		break;
	}
	case M_HORSECOLLAR:
		break;
	case M_HEAVYPLOW:
		break;
	case M_CROPROTATION:
		break;
	case LC_DOUBLEBIT_AXE:
	case LC_BOW_SAW:
	case LC_TWOMAN_SAW:
	{
		float val = 1.2;
		if (research_type == LC_TWOMAN_SAW)val = 1.1;
		uint size = App->entities_manager->ally_units_defs.size();
		for (uint k = 0; k < size; k++)
		{
			if (App->entities_manager->ally_units_defs[k]->GetUnitType() == VILLAGER)
			{
				Villager* target = nullptr;
				if (diplomacy == ALLY)target = (Villager*)App->entities_manager->ally_units_defs[k];
				else target = (Villager*)App->entities_manager->enemy_units_defs[k];

				target->SetRecollectRate(target->GetRecollectRate(TREE) * val, TREE);

				break;
			}
		}
		std::list<Unit*>::const_iterator unit = App->entities_manager->units.begin();
		while (unit != App->entities_manager->units.end())
		{
			if (unit._Ptr->_Myval->GetUnitType() == VILLAGER && unit._Ptr->_Myval->GetDiplomacy() == diplomacy)
			{
				((Villager*)unit._Ptr->_Myval)->SetRecollectRate(((Villager*)unit._Ptr->_Myval)->GetRecollectRate(TREE) * val, TREE);
			}
			unit++;
		}
		break;
	}
	case MC_GOLD_MINING:
	case MC_GOLD_SHAFT:
	case MC_STONE_MINING:
	case MC_STONE_SHAFT:
	{
		RESOURCE_TYPE res_target_type = GOLD_ORE;
		if (research_type == MC_STONE_MINING || research_type == MC_STONE_SHAFT)res_target_type = STONE_ORE;

		uint size = App->entities_manager->ally_units_defs.size();
		for (uint k = 0; k < size; k++)
		{
			if (App->entities_manager->ally_units_defs[k]->GetUnitType() == VILLAGER)
			{
				Villager* target = nullptr;
				if (diplomacy == ALLY)target = (Villager*)App->entities_manager->ally_units_defs[k];
				else target = (Villager*)App->entities_manager->enemy_units_defs[k];

				target->SetRecollectRate(target->GetRecollectRate(res_target_type) * 1.15, res_target_type);

				break;
			}
		}
		std::list<Unit*>::const_iterator unit = App->entities_manager->units.begin();
		while (unit != App->entities_manager->units.end())
		{
			if (unit._Ptr->_Myval->GetUnitType() == VILLAGER && unit._Ptr->_Myval->GetDiplomacy() == diplomacy)
			{
				((Villager*)unit._Ptr->_Myval)->SetRecollectRate(((Villager*)unit._Ptr->_Myval)->GetRecollectRate(res_target_type) * 1.15, res_target_type);
			}
			unit++;
		}
		break;
	}
	}
	return false;
}

Unit * j1EntitiesManager::GetNearestUnit(iPoint point, DIPLOMACY diplomacy, int max_dist, UNIT_TYPE type)
{
	Unit* ret = nullptr;

	std::list<Unit*>::const_iterator unit_it = units.begin();
	uint distance = 120 * 120 * 50;


	while (unit_it != units.end())
	{
	
		//Check unit type and diplomacy 
		//If unit type == NO_UNIT only check diplomacy
		if ((unit_it._Ptr->_Myval->GetUnitType() != type && type != UNIT_TYPE::NO_UNIT ) || unit_it._Ptr->_Myval->GetDiplomacy() != diplomacy)
		{
			unit_it++;
			continue;
		}


		//Calculate distance between resource pos & point
		uint dist = abs(unit_it._Ptr->_Myval->GetPositionRounded().DistanceTo(point));
		//Check if you have a distance limier
		if (max_dist > 0)
		{
			if (dist > max_dist) continue;
		}

		//Check if is the nearest resource from the point 
		if (dist < distance)
		{
			distance = dist;
			ret = unit_it._Ptr->_Myval;
		}

		unit_it++;
	}

	return ret;

}

Building * j1EntitiesManager::GetNearestBuilding(iPoint point, BUILDING_TYPE type, DIPLOMACY diplomacy, int max_dist)
{
	Building* ret = nullptr;

	std::list<Building*>::const_iterator building_it = buildings.begin();
	uint distance = 120 * 120 * 50;


	while (building_it != buildings.end())
	{
		//Check resource type
		if (building_it._Ptr->_Myval->GetBuildingType() != type || building_it._Ptr->_Myval->GetDiplomacy() != diplomacy)
		{
			building_it++;
			continue;
		}

		//Calculate distance between resource pos & point
		uint dist = abs(building_it._Ptr->_Myval->GetPositionRounded().DistanceTo(point));

		//Check if you have a distance limier
		if (max_dist > 0)
		{
			if (dist > max_dist) continue;
		}

		//Check if is the nearest resource from the point 
		if (dist < distance)
		{
			distance = dist;
			ret = building_it._Ptr->_Myval;
		}

		building_it++;
	}

	return ret;
}

Building * j1EntitiesManager::GetNearestSavePoint(iPoint position, PLAYER_RESOURCES resource_to_save, DIPLOMACY diplomacy)
{
	if (resource_to_save == GP_NO_RESOURCE)return nullptr;
	Building* ret = nullptr;

	std::list<Building*>::const_iterator building_it = buildings.begin();
	uint distance = 120 * 120 * 50;


	while (building_it != buildings.end())
	{
		bool correct_type = false;
		switch (resource_to_save)
		{
		case GP_WOOD:
			correct_type = (building_it._Ptr->_Myval->GetBuildingType() == BUILDING_TYPE::LUMBER_CAMP);
			break;
		case GP_GOLD:
		case GP_STONE:
			correct_type = (building_it._Ptr->_Myval->GetBuildingType() == BUILDING_TYPE::MINING_CAMP);
			break;
		}
		if(!correct_type)correct_type = (building_it._Ptr->_Myval->GetBuildingType() == TOWN_CENTER || building_it._Ptr->_Myval->GetBuildingType() == TOWN_CENTER_C);
		
		//Check resource type
		if (!correct_type || building_it._Ptr->_Myval->GetDiplomacy() != diplomacy)
		{
			building_it++;
			continue;
		}

		//Calculate distance between resource pos & point
		uint dist = abs(building_it._Ptr->_Myval->GetPositionRounded().DistanceOctile(position));
		//Check if is the nearest resource from the point 
		if (dist < distance)
		{
			distance = dist;
			ret = building_it._Ptr->_Myval;
		}

		building_it++;
	}

	return ret;
}

Resource * j1EntitiesManager::GetNearestResource(iPoint point, RESOURCE_TYPE type, DIPLOMACY diplomacy, int max_dist)
{
	Resource* ret = nullptr;

	std::list<Resource*>::const_iterator resource = resources.begin();
	uint distance = 120 * 120 * 50;


	while (resource != resources.end())
	{
		//Check resource type
		if (resource._Ptr->_Myval->GetResourceType() != type || resource._Ptr->_Myval->GetDiplomacy() != diplomacy)
		{
			resource++;
			continue;
		}

		//Calculate distance between resource pos & point
		uint dist = abs(resource._Ptr->_Myval->GetPositionRounded().DistanceTo(point));
		
		//Check if you have a distance limier
		if (max_dist > 0)
		{
			if (dist > max_dist)
			{
				resource++;
				continue;
			}
		}

		//Check if is the nearest resource from the point 
		if (dist < distance)
		{
			distance = dist;
			ret = resource._Ptr->_Myval;
		}

		resource++;
	}

	return ret;
}

bool j1EntitiesManager::CheckAreaToSpawn(const Circle & area) const
{
	std::vector<iPoint> tiles;
	uint size = App->map->map_quadtree.CollectCandidates(tiles, area);

	for (uint k = 0; k < size; k++)
	{
		if (!App->pathfinding->IsWalkable(tiles[k]) || !App->map->CheckConstructionMap(tiles[k], 1, 1))
		{
			return false;
		}
	}

	//Check unit quadtree for units in zone
	std::vector<Unit*> in_zone;
	App->entities_manager->units_quadtree.CollectCandidates(in_zone, area);
	if (!in_zone.empty())
	{
		return false;
	}

	return true;
}

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

void j1EntitiesManager::Disable()
{
	active = false;

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
	
	//Clean Up death units list (already deleted with Units CleanUp)
	death_units.clear();

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

	//Clear champions list
	champions_blue.clear();
	champions_red.clear();

}

bool j1EntitiesManager::Start()
{
	//Load Civilization Test
	bool ret = LoadCivilization("Teutons.xml");

	//Built entities quad trees
	units_quadtree.SetMaxObjects(8);
	units_quadtree.SetDebugColor({ 255,0,255,255 });
	resources_quadtree.SetMaxObjects(8);
	resources_quadtree.SetDebugColor({ 0,255,255,255 });
	buildings_quadtree.SetMaxObjects(3);
	buildings_quadtree.SetDebugColor({ 255,255,0,255 });


	return ret;
}

bool j1EntitiesManager::Update(float dt)
{
	//Update all the units
	std::list<Unit*>::const_iterator item = units.begin();
	bool ret = true;

	while (item != units.end())
	{
		ret = item._Ptr->_Myval->Update();
		item++;
	}

	std::list<Building*>::const_iterator item_build = buildings.begin();

	while (item_build != buildings.end())
	{

		ret = item_build._Ptr->_Myval->Update();

		item_build++;
	}

	return ret;
}

bool j1EntitiesManager::PostUpdate()
{
	//If there's no entities to delete returns
	if (wasted_units.empty())return true;

	//Clean all the wasted entities
	uint size = wasted_units.size();
	for (uint k = 0; k < size; k++)
	{
		//Get the entity type
		ENTITY_TYPE type = wasted_units[k]->GetEntityType();

		//Remove the entity from the correct list
		if (type == RESOURCE)
		{
			resources.remove((Resource*)wasted_units[k]);
			resources_quadtree.Exteract((Resource*)wasted_units[k], &wasted_units[k]->GetPosition());
		}
		else if (type == BUILDING)
		{
			buildings.remove((Building*)wasted_units[k]);
			buildings_quadtree.Exteract((Building*)wasted_units[k], &wasted_units[k]->GetPosition());
		}
		else if (type == UNIT)
		{
			wasted_units[k]->GetWorker()->HardReset();
			units.remove((Unit*)wasted_units[k]);
			death_units.remove((Unit*)wasted_units[k]);
			units_quadtree.Exteract((Unit*)wasted_units[k], &wasted_units[k]->GetPosition());

		}

		RELEASE(wasted_units[k]);
	}
	wasted_units.clear();

	return true;
}

bool j1EntitiesManager::Draw() const
{
	bool ret = true;

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
		if (fog_type == DARK_FOG)continue;
		if (units_vec[k]->GetDiplomacy() == ENEMY && fog_type != NO_FOG)continue;
		

		units_vec[k]->Draw(App->debug_mode);
	}

	std::list<Unit*>::const_iterator death_unit = death_units.begin();
	for (; death_unit != death_units.end(); death_unit++)
	{
		death_unit._Ptr->_Myval->Draw(App->debug_mode);
	}
	//Draw all Resources
	std::vector<Resource*> resources_vec;
	resources_quadtree.CollectCandidates(resources_vec, App->render->camera_viewport);
	size = resources_vec.size();
	for (uint k = 0; k < size; k++)
	{
		pos = resources_vec[k]->GetPositionRounded();
		pos = App->map->WorldToMap(pos.x, pos.y);
		if (App->fog_of_war->GetFogID(pos.x, pos.y) != DARK_FOG)resources_vec[k]->Draw(App->debug_mode);
	}

	//Draw all buildings
	std::vector<Building*> buildings_vec;
	buildings_quadtree.CollectCandidates(buildings_vec, App->render->camera_viewport);
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
	//Clean Up Units List
	std::list<Unit*>::iterator units_item = units.begin();
	while (units_item != units.end())
	{
		switch (units_item._Ptr->_Myval->GetUnitType())
		{
		case VILLAGER:
			delete ((Villager*)units_item._Ptr->_Myval);
			break;
		case WARRIOR_CHMP:
			delete ((Warrior*)units_item._Ptr->_Myval);
			break;
		case WIZARD_CHMP:
			delete ((Wizard*)units_item._Ptr->_Myval);
			break;
		default:
			RELEASE(units_item._Ptr->_Myval);
			break;
		}
		units_item++;
	}
	units.clear();
	units_quadtree.Reset();
	units_quadtree.Clear();

	//Clean Up resources list
	std::list<Resource*>::iterator resources_item = resources.begin();
	while (resources_item != resources.end())
	{
		RELEASE(resources_item._Ptr->_Myval);
		resources_item++;
	}
	resources.clear();
	resources_quadtree.Reset();
	resources_quadtree.Clear();

	//Clean Up buildings list
	std::list<Building*>::iterator buildings_item = buildings.begin();
	while (buildings_item != buildings.end())
	{
		RELEASE(buildings_item._Ptr->_Myval);
		buildings_item++;
	}
	buildings.clear();
	buildings_quadtree.Reset();
	buildings_quadtree.Clear();

	//Clean Up units_defs vector
	uint size = ally_units_defs.size();
	for (uint k = 0; k < size; k++)
	{
		switch (ally_units_defs[k]->GetUnitType())
		{
		case VILLAGER:
			delete ((Villager*)ally_units_defs[k]);
			delete ((Villager*)enemy_units_defs[k]);
			break;
		case WARRIOR_CHMP:
			delete ((Warrior*)ally_units_defs[k]);
			delete ((Warrior*)enemy_units_defs[k]);
			break;
		default:
			RELEASE(ally_units_defs[k]);
			RELEASE(enemy_units_defs[k]);
			break;
		}
	}
	ally_units_defs.clear();
	enemy_units_defs.clear();

	//Clean Up resoureces_defs vector
	size = resources_defs.size();
	for (uint k = 0; k < size; k++)
	{
		RELEASE(resources_defs[k]);
	}
	resources_defs.clear();

	//Clean Up buildings_defs vector
	size = buildings_defs.size();
	for (uint k = 0; k < size; k++)
	{
		RELEASE(buildings_defs[k]);
	}
	buildings_defs.clear();


	return true;
}

bool j1EntitiesManager::Load(pugi::xml_node& data)
{
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
		
		Resource* new_res = App->entities_manager->GenerateResource((RESOURCE_TYPE)cur_res_node.attribute("res_type").as_int());
		
		//Load position
		new_res->SetPosition(cur_res_node.attribute("pos_x").as_float(), cur_res_node.attribute("pos_y").as_float());
	
		//Load life
		new_res->SetLife(cur_res_node.attribute("life").as_int());		

		//Focus the next resource
		cur_res_node  = cur_res_node.next_sibling();
	}
	// ------------------------------------------

	// Buildings Load ---------------------------
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

		//Load building position
		new_building->SetPosition(cur_build_node.attribute("pos_x").as_float(), cur_build_node.attribute("pos_y").as_float());

		//Load building life
		new_building->SetLife(cur_build_node.attribute("life").as_uint());

		switch (building_type)
		{
		case TOWN_CENTER:
		case BARRACK:
		case STABLE:
		case ARCHERY_RANGE:
			/*Load productive building data*/
			/*
			current_units
			units_in <list>

			production_queue <priority_queue>
			*/

			//Load building current units count
			((ProductiveBuilding*)new_building)->SetCurrentUnits(cur_build_node.attribute("cur_units_in").as_uint());

			if (((ProductiveBuilding*)new_building)->GetCurrentUnits() > 0)
			{
				//Node where covered units are loaded
				pugi::xml_node units_in_node = cur_build_node.child("units_in");

				//Node focusing the current unit in
				pugi::xml_node unit_in_node = units_in_node.first_child();

				while (unit_in_node != NULL)
				{
					//Generate a unit in with type
					Unit* new_unit_in = GenerateUnit((UNIT_TYPE)unit_in_node.attribute("unit_type").as_int(), ALLY, false);

					//Load current unit in life
					new_unit_in->SetLife(unit_in_node.attribute("life").as_uint());

					//Focus next unit in node
					unit_in_node = unit_in_node.next_sibling();

				}
			}

			//Load building production queue (or not)

			break;

		default:
			/**/
			break;
		}

		//Focus the next building node
		cur_build_node = cur_build_node.next_sibling();
	}
	// ------------------------------------------

	return true;
}

bool j1EntitiesManager::Save(pugi::xml_node& data) const
{
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

		//Save building position
		cur_build_node.append_attribute("pos_x") = current_building._Ptr->_Myval->GetPosition().x;
		cur_build_node.append_attribute("pos_y") = current_building._Ptr->_Myval->GetPosition().y;

		//Save building life
		cur_build_node.append_attribute("life") = current_building._Ptr->_Myval->GetLife();

		//Save building diplomacy
		cur_build_node.append_attribute("diplomacy") = current_building._Ptr->_Myval->GetDiplomacy();

		//Save building type
		BUILDING_TYPE type = current_building._Ptr->_Myval->GetBuildingType();
		cur_build_node.append_attribute("build_type") = type;

		switch (type)
		{
		case TOWN_CENTER:
		case BARRACK:
		case STABLE:
		case ARCHERY_RANGE:
			/*Save productive building data*/
			/*
			current_units
			units_in <list>

			production_queue <priority_queue>
			*/

			//Save building current units count
			cur_build_node.append_attribute("cur_units_in") = ((ProductiveBuilding*)current_building._Ptr->_Myval)->GetCurrentUnits();
			
			if (((ProductiveBuilding*)current_building._Ptr->_Myval)->GetCurrentUnits() > 0)
			{
				//Node where covered units are saved
				pugi::xml_node units_in_node = cur_build_node.append_child("units_in");

				//Iterate units in list to save units data
				std::list<Unit*> units_in = ((ProductiveBuilding*)current_building._Ptr->_Myval)->GetUnitsIn();
				std::list<Unit*>::const_iterator unit_in = units_in.begin();
				while (unit_in != units_in.begin())
				{
					//Node where the current unit in is saved
					pugi::xml_node cur_unit_in_node = units_in_node.append_child("unit_in");

					//Save unit in unit type
					cur_unit_in_node.append_attribute("unit_type") = unit_in._Ptr->_Myval->GetUnitType();

					//Save unit in life
					cur_unit_in_node.append_attribute("life") = unit_in._Ptr->_Myval->GetLife();

					//Focus the next unit in
					unit_in++;
				}

			}

			//Save building production queue (or not)

			break;

		default:
			/**/
			break;
		}

		//Focus the next building
		current_building++;
	}
	// ------------------------------------------

	// Units Save -------------------------------
	//Units node
	pugi::xml_node units_node = data.append_child("units");

	//Iterate all the current buildings
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
			/*
			- level
			- ability [3]
			- actived[3];
			- ability_lvl_2_current_time
			- ability_lvl_3_current_time
			*/
			break;
		case ARCHER_CHMP:
			/*
			- ability [3]
			- actived[3];
			- ability_lvl_2_current_time
			- ability_lvl_3_current_time
			*/
			break;
		case WIZARD_CHMP:
			/*
			- ability [3]
			- actived[3];
			- ability_lvl_2_current_time
			- ability_lvl_3_current_time
			*/
			break;
		}

		//Focus the next unit
		current_unit++;
	}

	// ------------------------------------------
	return true;
}

//Methods to add entities definitions
bool j1EntitiesManager::AddUnitDefinition(const pugi::xml_node* unit_node)
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
	/*View Area*/		new_def->SetViewArea(unit_node->attribute("view_area").as_uint());
	/*Speed*/			new_def->SetSpeed(unit_node->attribute("speed").as_float());
	/*Attack Delay*/	new_def->SetAttackDelay(unit_node->attribute("attack_delay").as_uint());
	/*Attack Points*/	new_def->SetAttackHitPoints(unit_node->attribute("attack_hitpoints").as_uint());
	/*Attack Bonus*/	new_def->SetAttackBonus(unit_node->attribute("attack_bonus").as_uint());
	/*Siege Points*/	new_def->SetSiegeHitPoints(unit_node->attribute("siege_hitpoints").as_uint());
	/*Attack Rate*/		new_def->SetAttackRate(unit_node->attribute("attack_rate").as_uint());
	/*Attack Area*/		Circle area({ 0,0 }, unit_node->attribute("attack_range").as_uint(), { 0,0 });
						area.SetColor({ 0, 0, 255, 255 });
						new_def->SetAttackArea(area);
	/*Defense*/			new_def->SetDefense(unit_node->attribute("defense").as_uint());
	/*Defense Bonus*/	new_def->SetDefenseBonus(unit_node->attribute("defense_bonus").as_uint());
	/*Armor*/			new_def->SetArmor(unit_node->attribute("armor").as_uint());
	/*Armor Bonus*/		new_def->SetArmorBonus(unit_node->attribute("armor_bonus").as_uint());
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
		/*Recollect Rate*/			((Villager*)new_def)->SetRecollectRate(unit_node->attribute("recollect_rate").as_uint());
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
									atk_triangle.SetXAngle(unit_node->attribute("area_triangle_X_angle").as_float());
									((Hunter*)new_def)->SetSpecialAttackArea(atk_triangle);
		/*Circle Spell*/			Circle temp_circle;
									temp_circle.SetRad(unit_node->attribute("area_attack_skill_B_lvl_2").as_uint());
									temp_circle.SetColor({ 0,50,50,255 });
									((Hunter*)new_def)->SetSpecialAttackArea(temp_circle, "area_lvl_2");
									temp_circle.SetRad(unit_node->attribute("area_attack_skill_A_lvl_3").as_uint());
									temp_circle.SetColor({ 0,50,50,255 });
									((Hunter*)new_def)->SetSpecialAttackArea(temp_circle, "area_lvl_3");
									SDL_Rect temp_rect;
									temp_rect.w = unit_node->attribute("rect_attack_w").as_uint();
									temp_rect.h = unit_node->attribute("rect_attack_h").as_uint();
									((Hunter*)new_def)->SetSpecialAttackArea(temp_rect);
									((Hunter*)new_def)->SetAbility_lvl_2_A_AttackValue(unit_node->attribute("ability_lvl_2_skill_A_attack_value").as_uint());
									((Hunter*)new_def)->SetAbility_lvl_2_B_AttackValue(unit_node->attribute("ability_lvl_2_skill_B_attack_value").as_uint());
									((Hunter*)new_def)->SetAbility_lvl_3_A_AttackValue(unit_node->attribute("ability_lvl_3_skill_A_attack_value").as_uint());
									((Hunter*)new_def)->SetAbility_lvl_3_B_AttackValue(unit_node->attribute("ability_lvl_3_skill_B_attack_value").as_uint());
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
		/*Armor for level*/			((Champion*)new_def)->SetArmorForLevel(unit_node->attribute("armor_for_level").as_uint());
		/*Defense for level*/		((Champion*)new_def)->SetDefenseForLevel(unit_node->attribute("defense_for_level").as_float());
		/*Speed for level*/			((Champion*)new_def)->SetSpeedForLevel(unit_node->attribute("speed_for_level").as_float());
		/*View Area for level*/		((Champion*)new_def)->SetViewAreaForLevel(unit_node->attribute("view_area_for_level").as_uint());
	
	}

	//Add the generated unit in the units definitions entities manager array
	ally_units_defs.push_back(new_def);
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

	LOG("%s definition built!", new_def->GetName());
	
	return true;
}

bool j1EntitiesManager::AddResourceDefinition(const pugi::xml_node * resource_node)
{
	if (resource_node == nullptr)return false;

	//Generate a new resource definition from the node
	Resource* new_def = new Resource();
	
	//Resource ID -----------
	/*Name*/			new_def->SetName(resource_node->attribute("name").as_string());
	/*Entity Type*/		new_def->SetEntityType(RESOURCE);
	/*Resource Type*/	new_def->SetResourceType(App->animator->StrToResourceEnum(resource_node->attribute("resource_type").as_string()));
	
	//Resource Primitives ---
	/*Mark*/			Rectng mark;
	/*Mark Width*/		mark.SetWidth(resource_node->attribute("mark_w").as_uint());
	/*Mark Height*/		mark.SetHeight(resource_node->attribute("mark_h").as_uint());
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

	resources_defs.push_back(new_def);

	LOG("%s definition built!", new_def->GetName());

	return true;
}

bool j1EntitiesManager::AddBuildingDefinition(const pugi::xml_node * building_node)
{
	if (building_node == nullptr)return false;

	//Get building type to allocate the necessary attributes
	BUILDING_TYPE building_type = App->animator->StrToBuildingEnum(building_node->attribute("building_type").as_string());

	//Generate a new building definition from the node
	Building* new_def = nullptr;

	//Allocate the correct class

	new_def = new ProductiveBuilding();
	

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
	/*Mark Height*/		mark.SetHeight(building_node->attribute("mark_h").as_uint());
	/*Mark Displace*/	iPoint displacement(building_node->attribute("mark_x").as_int(), building_node->attribute("mark_y").as_int());
						mark.SetDisplacement(displacement);
	/*Mark Color*/		mark.SetColor({ 55,255,255,255 });
						new_def->SetMark(mark);
	/*Interaction Area*/Rectng area;
	/*I.Area Width*/	area.SetWidth(building_node->attribute("interaction_area_w").as_uint());
	/*I.Area Height*/	area.SetHeight(building_node->attribute("intaraction_area_h").as_uint());
	/*I.Area Displace*/	displacement.create(building_node->attribute("interaction_area_x").as_int(), building_node->attribute("intaraction_area_y").as_int());
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

	if (building_type == TOWN_CENTER || building_type == BARRACK || building_type == STABLE || building_type == ARCHERY_RANGE)
	{
		/*Units Capacity*/	((ProductiveBuilding*)new_def)->SetUnitsCapacity(building_node->attribute("units_capacity").as_uint());
		/*Units Spawn pnt*/	iPoint spawn(building_node->attribute("units_spawn_x").as_int(), building_node->attribute("units_spawn_y").as_int());
							((ProductiveBuilding*)new_def)->SetUnitsSpawnPoint(spawn);
		/*Production Cap*/	((ProductiveBuilding*)new_def)->SetProductionCapacity(building_node->attribute("production_capacity").as_uint());

	}
	buildings_defs.push_back(new_def);

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
bool j1EntitiesManager::LoadCivilization(const char * folder)
{
	j1Timer time;
	time.Start();

	//Load Civilization Animations --------------
	//Load civilization data from loaded folder
	LOG("---- Loading %s...", folder);
	std::string load_folder = name + "/" + folder;
	pugi::xml_document civilization_data;
	if (!App->fs->LoadXML(load_folder.c_str(), &civilization_data))
	{
		LOG("Civilization animations load error!");
		return false;
	}

	//Get Civilization name
	std::string civ_name = civilization_data.first_child().first_attribute().as_string();
	//Boolean to check the correct file loads
	bool ret = true;
	//Load civilization units list
	pugi::xml_node unit_node = civilization_data.first_child().child("units").first_child();
	while (unit_node != NULL)
	{
		if (!ret)break;
		ret = App->animator->LoadUnitBlock(unit_node.attribute("xml").as_string());
		unit_node = unit_node.next_sibling();
	}
	//Load civilization buildings list
	pugi::xml_node building_node = civilization_data.child("data").child("buildings").first_child();
	while (building_node != NULL)
	{
		if (!ret)break;
		ret = App->animator->LoadBuildingBlock(building_node.attribute("xml").as_string());
		building_node = building_node.next_sibling();
	}
	//Load civilization resources
	pugi::xml_node resource_node = civilization_data.first_child().child("resources").first_child();
	if(resource_node != NULL)ret = App->animator->LoadResourceBlock(resource_node.attribute("xml").as_string());
	else LOG("Error loading civilization Resources");
	// ------------------------------------------

	//Load Civilization Stats -------------------
	//Load stats document
	civilization_data.reset();
	load_folder.clear();
	load_folder = name + "/" + "StatsData.xml";
	if (!App->fs->LoadXML(load_folder.c_str(), &civilization_data))
	{
		LOG("Civilization stats load error!");
		return false;
	}

	//Focus first entity node
	pugi::xml_node entity_node = civilization_data.child("data").first_child();

	while (entity_node != NULL)
	{
		//Check if the entity is in the chosen civilization
		if (!CivilizationCheck((char*)entity_node.attribute("civ").as_string(), civ_name.c_str()))
		{
			entity_node = entity_node.next_sibling();
			continue;
		}

		//If the entity is in the civilization the entity definition is loaded
		switch (App->animator->StrToEntityEnum(entity_node.attribute("id").as_string()))
		{
		case UNIT:		AddUnitDefinition(&entity_node.first_child());		break;
		case RESOURCE:	AddResourceDefinition(&entity_node.first_child());	break;
		case BUILDING:	AddBuildingDefinition(&entity_node.first_child());	break;

		default:
			//Entity ID error case
			LOG("Error loading entity %s", entity_node.first_child().attribute("name").as_string());
			break;
		}

		//Focus the next entity node
		entity_node = entity_node.next_sibling();
	}
	// ------------------------------------------

	//Load xml sounds
	load_folder.clear();

	load_folder = name + "/" + "SoundsData.xml";
	App->sound->LoadSoundBlock(load_folder.c_str());

	// ------------------------------------------

	//Clean loaded xml
	civilization_data.reset();

	LOG("---- %s loaded in %.3f", folder, time.ReadSec());

	return ret;
}

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
				if(diplomacy == ALLY)new_villager = new Villager(*(Villager*)ally_units_defs[k]);
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

			//Set new unit myself pointer
			new_unit->myself = new_unit;

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

	uint def_num = buildings_defs.size();
	for (uint k = 0; k < def_num; k++)
	{
		if (buildings_defs[k]->GetBuildingType() == type)
		{
			if (type == TOWN_CENTER || type == BARRACK || type == STABLE || type == ARCHERY_RANGE)
			{	
				new_building = new ProductiveBuilding(*(ProductiveBuilding*)buildings_defs[k]);
				
			}
			else
			{
				//Build unit
				new_building = new Building(*buildings_defs[k]);
			}
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

			//Set building myself pointer
			new_building->myself = new_building;

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
			new_resource = new Resource(*resources_defs[k]);

			new_resource->myself = new_resource;

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
	
	//Add the entity at the wasted entities list 
	wasted_units.push_back(entity);

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

void j1EntitiesManager::AddUnit(Unit* unit)
{
	units.push_back((Unit*)unit);
	units_quadtree.Exteract(unit,&unit->GetPosition());
	units_quadtree.Insert((Unit*)unit, &unit->GetPosition());
}

void j1EntitiesManager::AddDeathUnit(Unit * unit)
{

	death_units.push_back((Unit*)unit);
	units_quadtree.Exteract(unit, &unit->GetPosition());

}

void j1EntitiesManager::ResurrectUnit(Unit * unit)
{
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

Building * j1EntitiesManager::SearchNearestSavePoint(const iPoint & point)
{
	std::list<Building*>::const_iterator building = buildings.begin();
	uint distance = 120 * 120 * 50;
	Building* nearest_building = nullptr;
	while (building != buildings.end())
	{
		//Check building type
		if (building._Ptr->_Myval->GetBuildingType() != TOWN_CENTER)
		{
			building++;
			continue;
		}
		//Calculate distance between building pos & point
		uint dist = abs(building._Ptr->_Myval->GetPositionRounded().DistanceNoSqrt(point));
		//Check if is the nearest building from the point 
		if ( dist < distance)
		{
			distance = dist;
			nearest_building = building._Ptr->_Myval;
		}

		building++;
	}

	return nearest_building;
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
		break;
	}
}

bool j1EntitiesManager::UpgradeUnit(UNIT_TYPE u_type, UNIT_TYPE new_type, DIPLOMACY e_diplomacy)
{
	bool ret = false;

	if (u_type != NO_UNIT && new_type != NO_UNIT)
	{
		Unit* new_unit = nullptr;
		int size = ally_units_defs.size();
		for (int count = 0; count < size; count++)
		{
			if (ally_units_defs[count]->GetUnitType() == new_type)
			{
				new_unit = ally_units_defs[count];
				break;
			}
		}
		std::list<Unit*>::iterator unit = units.begin();
		while (unit != units.end())
		{
			if (unit._Ptr->_Myval->GetDiplomacy() == e_diplomacy && unit._Ptr->_Myval->GetUnitType() == u_type)
			{
				unit._Ptr->_Myval->SetUpgrade(new_unit);
			}
			unit++;
		}
	}

	return ret;
}

Resource * j1EntitiesManager::GetNearestResource(iPoint point, RESOURCE_TYPE type)
{
	Resource* ret = nullptr;

	std::list<Resource*>::const_iterator resource = resources.begin();
	uint distance = 120 * 120 * 50;


	while (resource != resources.end())
	{
		//Check resource type
		if (resource._Ptr->_Myval->GetResourceType() != type)
		{
			resource++;
			continue;
		}

		//Calculate distance between resource pos & point
		uint dist = abs(resource._Ptr->_Myval->GetPositionRounded().DistanceNoSqrt(point));
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

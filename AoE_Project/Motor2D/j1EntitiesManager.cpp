#define _CRT_SECURE_NO_WARNINGS

#include "j1EntitiesManager.h"

#include "j1App.h"
#include "j1Render.h"
#include "j1Animator.h"
#include "j1Input.h"
#include "j1FileSystem.h"

#include "p2Log.h"

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

bool j1EntitiesManager::Start()
{
	//Load Civilization Test
	bool ret = LoadCivilization("Teutons.xml");
	return ret;
}

bool j1EntitiesManager::Update(float dt)
{
	//Activate / Deactivate debug mode 
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) debug = !debug;

	return true;
}

bool j1EntitiesManager::Draw() const
{
	bool ret = true;

	//Draw all units
	std::list<Unit*>::const_iterator unit_item = units.begin();
	while (unit_item != units.end() && ret)
	{
		ret = unit_item._Ptr->_Myval->Draw(debug);
		unit_item++;
	}
	//Draw all Resources
	std::list<Resource*>::const_iterator resource_item = resources.begin();
	while (resource_item != resources.end() && ret)
	{
		ret = resource_item._Ptr->_Myval->Draw(debug);
		resource_item++;
	}
	//Draw all buildings
	std::list<Building*>::const_iterator building_item = buildings.begin();
	while (building_item != buildings.end() && ret)
	{
		ret = building_item._Ptr->_Myval->Draw(debug);
		building_item++;
	}

	return ret;
}

bool j1EntitiesManager::CleanUp()
{
	//Clean Up Units List
	std::list<Unit*>::iterator units_item = units.begin();
	while (units_item != units.end())
	{
		RELEASE(units_item._Ptr->_Myval);
		units_item++;
	}
	units.clear();

	//Clean Up resources list
	std::list<Resource*>::iterator resources_item = resources.begin();
	while (resources_item != resources.end())
	{
		RELEASE(resources_item._Ptr->_Myval);
		resources_item++;
	}
	resources.clear();

	//Clean Up buildings list
	std::list<Building*>::iterator buildings_item = buildings.begin();
	while (buildings_item != buildings.end())
	{
		RELEASE(buildings_item._Ptr->_Myval);
		buildings_item++;
	}
	buildings.clear();

	//Clean Up units_defs vector
	units_defs.clear();

	//Clean Up resoureces_defs vector
	resources_defs.clear();

	//Clean Up buildings_defs vector
	buildings_defs.clear();

	return true;
}

//Methods to add entities definitions
bool j1EntitiesManager::AddUnitDefinition(const pugi::xml_node* unit_node)
{
	if (unit_node == nullptr)return false;

	//Generate a new unit definition from the node
	Unit new_def;
	
	//Unit ID ---------------
	/*Name*/			new_def.SetName(unit_node->attribute("name").as_string());
	/*Entity Type*/		new_def.SetEntityType(UNIT);
	/*Unit Type*/		new_def.SetUnitType(App->animator->StrToUnitEnum(unit_node->attribute("unit_type").as_string()));
	/*Attack Type*/		new_def.SetAttackType(App->animator->StrToAttackEnum(unit_node->attribute("attack_type").as_string()));
	//Unit Primitives -------
	/*Mark*/			Circle mark;
	/*Mark Radius*/		mark.SetRad(unit_node->attribute("mark_rad").as_uint());
	/*Mark X Angle*/	mark.SetXAngle(unit_node->attribute("mark_x_angle").as_float());
						mark.SetColor({ 255,255,255,255 });
						new_def.SetMark(mark);
	/*Selection Rect*/	SDL_Rect selection_rect;
	/*S.Rect X*/		selection_rect.x = unit_node->attribute("selection_x").as_int();
	/*S.Rect Y*/		selection_rect.y = unit_node->attribute("selection_y").as_int();
	/*S.Rect W*/		selection_rect.w = unit_node->attribute("selection_w").as_int();
	/*S.Rect H*/		selection_rect.h = unit_node->attribute("selection_h").as_int();
						new_def.SetSelectionRect(selection_rect);
	/*Icon Rect*/		SDL_Rect icon_rect;
	/*I.Rect X*/		icon_rect.x = unit_node->attribute("icon_x").as_int();
	/*I.Rect Y*/		icon_rect.y = unit_node->attribute("icon_y").as_int();
	/*I.Rect W*/		icon_rect.w = unit_node->attribute("icon_w").as_int();
	/*I.Rect H*/		icon_rect.h = unit_node->attribute("icon_h").as_int();
						new_def.SetIcon(icon_rect);

	//Unit Metrics ----------
	/*Max Life*/		new_def.SetMaxLife(unit_node->attribute("max_life").as_uint());
	/*Life*/			new_def.SetLife(unit_node->attribute("life").as_uint());
	/*View Area*/		new_def.SetViewArea(unit_node->attribute("view_area").as_uint());
	/*Speed*/			new_def.SetSpeed(unit_node->attribute("speed").as_float());
	/*Attack Points*/	new_def.SetAttackHitPoints(unit_node->attribute("attack_hitpoints").as_uint());
	/*Attack Bonus*/	new_def.SetAttackBonus(unit_node->attribute("attack_bonus").as_uint());
	/*Siege Points*/	new_def.SetSiegeHitPoints(unit_node->attribute("siege_hitpoints").as_uint());
	/*Attack Rate*/		new_def.SetAttackRate(unit_node->attribute("attack_rate").as_uint());
	/*Attack Range*/	new_def.SetAttackRange(unit_node->attribute("attack_range").as_uint());
	/*Defense*/			new_def.SetDefense(unit_node->attribute("defense").as_uint());
	/*Defense Bonus*/	new_def.SetDefenseBonus(unit_node->attribute("defense_bonus").as_uint());
	/*Armor*/			new_def.SetArmor(unit_node->attribute("armor").as_uint());
	/*Armor Bonus*/		new_def.SetArmorBonus(unit_node->attribute("armor_bomus").as_uint());
	/*Food Cost*/		new_def.SetFoodCost(unit_node->attribute("food_cost").as_uint());
	/*Wood Cost*/		new_def.SetWoodCost(unit_node->attribute("wood_cost").as_uint());
	/*Gold Cost*/		new_def.SetGoldCost(unit_node->attribute("gold_cost").as_uint());
	/*Population Cost*/	new_def.SetPopulationCost(unit_node->attribute("population_cost").as_uint());
	/*Train Time*/		new_def.SetTrainTime(unit_node->attribute("train_time").as_uint());

	units_defs.push_back(new_def);
	
	LOG("%s definition built!", new_def.GetName());
	
	return true;
}

bool j1EntitiesManager::AddResourceDefinition(const pugi::xml_node * resource_node)
{
	return true;
}

bool j1EntitiesManager::AddBuildingDefinition(const pugi::xml_node * building_node)
{
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


	//Clean loaded xml
	civilization_data.reset();

	LOG("---- %s loaded in %.3f", folder, time.ReadSec());

	return ret;
}

Unit * j1EntitiesManager::GenerateUnit(UNIT_TYPE type)
{
	Unit* new_unit = nullptr;

	uint def_num = units_defs.size();
	for (uint k = 0; k < def_num; k++)
	{
		if (units_defs[k].GetUnitType() == type)
		{
			//Build unit
			new_unit = new Unit(units_defs[k]);
			
			//Set unit animation
			App->animator->UnitPlay(new_unit);
			
			//Add the new unit at the units manage list
			units.push_back(new_unit);

			return new_unit;
		}
	}
	return nullptr;
}

Building * j1EntitiesManager::GenerateBuilding(BUILDING_TYPE type)
{
	Building* new_buidling = nullptr;

	switch (type)
	{
	case NO_BUILDING:	new_buidling = new Building("undefined");		 break;
	case FARM:
		break;
	case TOWN_CENTER:
		new_buidling = new Building("Town Center");
		new_buidling->SetEntityType(ENTITY_TYPE::BUILDING);
		new_buidling->SetBuildingType(BUILDING_TYPE::TOWN_CENTER);
		App->animator->BuildingPlay(new_buidling);

		break;
	}

	//Add new unit at the manager list
	if (new_buidling != nullptr)buildings.push_back(new_buidling);

	//Return the generated unit
	return new_buidling;
}

Resource * j1EntitiesManager::GenerateResource(RESOURCE_TYPE type)
{
	//Pointer to the new resource
	Resource* new_resource = nullptr;

	//Allocate resource class
	new_resource = new Resource();
	
	//Set resource type
	new_resource->SetEntityType(RESOURCE);
	new_resource->SetResourceType(type);

	//Randomly set the animation of the new resource checking the type
	App->animator->ResourcePlay(new_resource);

	//Add the new resource at the entity manager resources list
	resources.push_back(new_resource);

	return new_resource;
}

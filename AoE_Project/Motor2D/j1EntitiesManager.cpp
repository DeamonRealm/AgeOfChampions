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

//Methods that transform strings to enums (used when loading data from xml)
ENTITY_TYPE j1EntitiesManager::StrToEntityEnum(const char * str) const
{
	if (strcmp(str, "unit") == 0)		return UNIT;
	if (strcmp(str, "resource") == 0)	return RESOURCE;
	if (strcmp(str, "building") == 0)	return BUILDING;
	return NO_ENTITY;
}

//Methods to add entities definitions
bool j1EntitiesManager::AddUnitDefinition(const pugi::xml_node* unit_node)
{
	if (unit_node == nullptr)return false;

	//Generate a new unit definition from the node
	Unit new_def;
	
	//Unit ID ---------------
	/*Name*/			new_def.SetName(unit_node->attribute("name").as_string());
	/*Entity Type*/		new_def.SetEntityType((ENTITY_TYPE)unit_node->attribute("entity_type").as_uint());
	/*Unit Type*/		new_def.SetUnitType((UNIT_TYPE)unit_node->attribute("unit_type").as_uint());
	/*Attack Type*/		new_def.SetAttackType((ATTACK_TYPE)unit_node->attribute("attack_type").as_uint());

	//Unit Primitives -------
	/*Mark*/			Circle* mark = new Circle();
						new_def.SetMark(mark);
	/*Mark Radius*/		mark->SetRad(unit_node->attribute("mark_rad").as_uint());
	/*Mark X Angle*/	mark->SetXAngle(unit_node->attribute("mark_x_angle").as_float());
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
	/*Coin Cost*/		new_def.SetCoinCost(unit_node->attribute("coin_cost").as_uint());
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

	//Boolean to check the correct file loads
	bool ret = true;
	std::string tex_folder;
	std::string tex_file;
	//Load civilization units list
	pugi::xml_node unit_node = civilization_data.child("data").child("units").first_child();
	while (unit_node != NULL)
	{
		if (!ret)break;
		ret = App->animator->LoadUnitBlock(unit_node.attribute("xml").as_string(), unit_node.attribute("spritesheet").as_string());
		unit_node = unit_node.next_sibling();
	}
	//Load civilization buildings list
	pugi::xml_node building_node = civilization_data.child("data").child("buildings").first_child();
	while (building_node != NULL)
	{
		if (!ret)break;
		ret = App->animator->LoadBuildingBlock(building_node.attribute("xml").as_string(), building_node.attribute("spritesheet").as_string());
		building_node = building_node.next_sibling();
	}
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
		switch (StrToEntityEnum(entity_node.attribute("id").as_string()))
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
			new_unit = new Unit(&units_defs[k]);
			
			//Build unit mark
			Circle* mark = (Circle*)units_defs[k].GetMark();
			Circle* cpy = new Circle(*mark);
			new_unit->SetMark(cpy);
			
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
		new_buidling->SetAnimation(App->animator->BuildingPlay(TOWN_CENTER, IDLE, NO_DIRECTION));

		break;
	}

	//Add new unit at the manager list
	if (new_buidling != nullptr)buildings.push_back(new_buidling);

	//Return the generated unit
	return new_buidling;
}
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
	uint size = units_defs.size();
	if (size > 0)
	{
		size--;
		for (uint k = size; k >= 0; k--)
		{
			RELEASE(units_defs[k]);
		}
		units_defs.clear();
	}

	//Clean Up resoureces_defs vector
	size = resoureces_defs.size();
	if (size > 0)
	{
		size--;
		for (uint k = size; k >= 0; k--)
		{
			RELEASE(resoureces_defs[k]);
		}
		resoureces_defs.clear();
	}
	//Clean Up buildings_defs vector
	size = buildings_defs.size();
	if (size > 0)
	{
		size--;
		for (uint k = size; k >= 0; k--)
		{
			RELEASE(buildings_defs[k]);
		}
		buildings_defs.clear();
	}

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
	/*civilization_data.reset();
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
			LOG("Error loading entity id");
			civilization_data.reset();
			return false;
			break;
		}
		//Focus the next entity node
		entity_node = entity_node.next_sibling();
	}*/
	// ------------------------------------------


	//Clean loaded xml
	civilization_data.reset();

	LOG("---- %s loaded in %.3f", folder, time.ReadSec());

	return ret;
}

Unit * j1EntitiesManager::GenerateUnit(UNIT_TYPE type)
{
	Unit* new_unit = nullptr;

	switch (type)
	{
	case NO_UNIT: new_unit = new Unit("undefined"); break;

	case VILLAGER_PICK:
		new_unit = new Unit("Villager");
		new_unit->SetEntityType(ENTITY_TYPE::UNIT);
		new_unit->SetUnitType(UNIT_TYPE::VILLAGER_PICK);
		new_unit->SetAction(ACTION_TYPE::IDLE);
		new_unit->SetDirection(DIRECTION_TYPE::SOUTH);
		new_unit->SetSelectionRect({ 0,0,30,40 });
		App->animator->UnitPlay(new_unit);
		new_unit->GenerateMark();
		break;

	case VILLAGER:
		new_unit = new Unit("Villager");
		new_unit->SetEntityType(ENTITY_TYPE::UNIT);
		new_unit->SetUnitType(UNIT_TYPE::VILLAGER);
		new_unit->SetAction(ACTION_TYPE::IDLE);
		new_unit->SetDirection(DIRECTION_TYPE::SOUTH);
		new_unit->SetSelectionRect({ 0,0,30,40 });
		App->animator->UnitPlay(new_unit);
		new_unit->GenerateMark();
		break;

	case VILLAGER_PLOW:
		new_unit = new Unit("Villager");
		new_unit->SetEntityType(ENTITY_TYPE::UNIT);
		new_unit->SetUnitType(UNIT_TYPE::VILLAGER_PLOW);
		new_unit->SetAction(ACTION_TYPE::IDLE);
		new_unit->SetDirection(DIRECTION_TYPE::SOUTH);
		new_unit->SetSelectionRect({0,0,30,40});
		App->animator->UnitPlay(new_unit);
		new_unit->GenerateMark();
		break;

	case VILLAGER_AXE:
		new_unit = new Unit("Villager");
		new_unit->SetEntityType(ENTITY_TYPE::UNIT);
		new_unit->SetUnitType(UNIT_TYPE::VILLAGER_AXE);
		new_unit->SetAction(ACTION_TYPE::IDLE);
		new_unit->SetDirection(DIRECTION_TYPE::SOUTH);
		new_unit->SetSelectionRect({ 0,0,30,40 });
		App->animator->UnitPlay(new_unit);
		new_unit->GenerateMark();
		break;

	case VILLAGER_HAMMER:
		new_unit = new Unit("Villager");
		new_unit->SetEntityType(ENTITY_TYPE::UNIT);
		new_unit->SetUnitType(UNIT_TYPE::VILLAGER_HAMMER);
		new_unit->SetAction(ACTION_TYPE::IDLE);
		new_unit->SetDirection(DIRECTION_TYPE::SOUTH);
		new_unit->SetSelectionRect({ 0,0,30,40 });
		App->animator->UnitPlay(new_unit);
		new_unit->GenerateMark();
		break;

	case VILLAGER_BASKET:
		new_unit = new Unit("Villager");
		new_unit->SetEntityType(ENTITY_TYPE::UNIT);
		new_unit->SetUnitType(UNIT_TYPE::VILLAGER_BASKET);
		new_unit->SetAction(ACTION_TYPE::IDLE);
		new_unit->SetDirection(DIRECTION_TYPE::SOUTH);
		new_unit->SetSelectionRect({ 0,0,30,40 });
		App->animator->UnitPlay(new_unit);
		new_unit->GenerateMark();
		break;

	case VILLAGER_BOW:
		new_unit = new Unit("Villager");
		new_unit->SetEntityType(ENTITY_TYPE::UNIT);
		new_unit->SetUnitType(UNIT_TYPE::VILLAGER_BOW);
		new_unit->SetAction(ACTION_TYPE::IDLE);
		new_unit->SetDirection(DIRECTION_TYPE::SOUTH);
		new_unit->SetSelectionRect({ 0,0,30,40 });
		App->animator->UnitPlay(new_unit);
		new_unit->GenerateMark();
		break;

//Don't use this, it's not done and most likely it will break the code
	case VILLAGER_CARRY:
		new_unit = new Unit("Villager");
		new_unit->SetEntityType(ENTITY_TYPE::UNIT);
		new_unit->SetUnitType(UNIT_TYPE::VILLAGER_CARRY);
		new_unit->SetAction(ACTION_TYPE::IDLE);
		new_unit->SetDirection(DIRECTION_TYPE::SOUTH);
		new_unit->SetSelectionRect({ 0,0,30,40 });
		App->animator->UnitPlay(new_unit);
		new_unit->GenerateMark();
		break;
//------------------------

	case MILITIA:
		new_unit = new Unit("Militia");
		new_unit->SetEntityType(ENTITY_TYPE::UNIT);
		new_unit->SetUnitType(UNIT_TYPE::MILITIA);
		new_unit->SetAction(ACTION_TYPE::IDLE);
		new_unit->SetDirection(DIRECTION_TYPE::SOUTH);
		new_unit->SetSelectionRect({ 0,0,30,40 });
		App->animator->UnitPlay(new_unit);
		new_unit->GenerateMark();
	 break;

	case ARBALEST:
		new_unit = new Unit("Arbalest");
		new_unit->SetEntityType(ENTITY_TYPE::UNIT);
		new_unit->SetUnitType(UNIT_TYPE::ARBALEST);
		new_unit->SetAction(ACTION_TYPE::IDLE);
		new_unit->SetDirection(DIRECTION_TYPE::SOUTH);
		new_unit->SetSelectionRect({ 0,0,35,45 });
		App->animator->UnitPlay(new_unit);
		new_unit->GenerateMark();
	 break;


	}

	//Add new unit at the manager list
	if(new_unit != nullptr)units.push_back(new_unit);
	
	//Return the generated unit
	return new_unit;
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
		new_buidling->GenerateMark();
		break;
	}

	//Add new unit at the manager list
	if (new_buidling != nullptr)buildings.push_back(new_buidling);

	//Return the generated unit
	return new_buidling;
}
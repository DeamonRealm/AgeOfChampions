
#include "j1App.h"
#include "j1AI.h"
#include "j1EntitiesManager.h"
#include "j1FileSystem.h"
#include "BaseEntities.h"
#include "j1Map.h"
#include "j1ActionManager.h"
#include "Actions_Unit.h"
#include "Actions_Building.h"
#include "j1GroupMovement.h"
#include "Hud_GamePanel.h"

#include "j1Input.h"

#include <time.h>

j1AI::j1AI()
{
	name = "AI";
}

j1AI::~j1AI()
{
}

void j1AI::Enable()
{
	//ai_worker->AddAICommand(new WaitAICommand(80000));

	//active = LoadEnemies("EnemiesSpawn.xml");
	active = true;


	update_timer.Start();

	research_timer.Start();


	ai_starting_tc = (ProductiveBuilding*)App->entities_manager->GenerateBuilding(TOWN_CENTER, ENEMY);
	iPoint pos = App->map->MapToWorldCenter(110, 100);
	ai_starting_tc->SetPosition((float)pos.x, (float)pos.y);
	enemy_buildings.push_back(ai_starting_tc);


	//temporary enemy barracks
	pos = App->map->MapToWorldCenter(115, 95);
	Building* new_building;
	new_building = App->entities_manager->GenerateBuilding(BARRACK, ENEMY);
	new_building->SetPosition((float)pos.x, (float)pos.y);
	enemy_buildings.push_back(new_building);

	//ai_worker->AddAICommand(new MoveUnitsCommand(enemy_units, App->map->MapToWorld(99,99)));
}

void j1AI::Disable()
{
	active = false;
	ai_worker->Reset();
	enemy_units.clear();
}


bool j1AI::Awake(pugi::xml_node&)
{

	bool ret = true;

	ai_worker = new AIWorker();
	ai_research_worker = new ActionWorker();

	//Load Research;
	//Load Panels Data from loaded folder
	char* buffer = nullptr;
	int size = App->fs->Load("AI/ia_data.xml", &buffer);
	pugi::xml_document ai_data;
	pugi::xml_parse_result result = ai_data.load_buffer(buffer, size);
	RELEASE_ARRAY(buffer);

	LoadAIEntitiesData(ai_data.first_child());


	return ret;
}

bool j1AI::Start()
{

	return true;
}

bool j1AI::PreUpdate()
{



	return true;
}

bool j1AI::Update(float dt)
{
	ai_worker->Update();
	ai_research_worker->Update();
	
	UpdateResearch();
	

	//	WIP
	
	if (building_timer.Read() > 10000)
	{
		ManageConstrucion();

		building_timer.Start();
	}
	

	//only update every 2 seconds
	if (update_timer.Read() < 1000)
	{
		return true;
	}

	Resource* to_recolect = GetNearestNeed();
	//ai_worker->AddAICommand(new SendToRecollect(enemy_units, (Resource**)to_recolect->GetMe()));
	
	std::list<Unit*>::const_iterator unit_it = enemy_units.begin();
	while (unit_it != enemy_units.end())
	{
		if (to_recolect == nullptr) break;
		//Check resource type
		if (unit_it._Ptr->_Myval->GetUnitType() != VILLAGER)
		{
			unit_it++;
			continue;
		}

		if (!unit_it._Ptr->_Myval->GetWorker()->IsBusy(TASK_CHANNELS::PRIMARY))
		{
			unit_it._Ptr->_Myval->GetWorker()->AddAction(App->action_manager->RecollectAction((Villager*)unit_it._Ptr->_Myval, (Resource*)to_recolect), PRIMARY);
		}
		unit_it++;
	}


	ManageTroopsCreation();
	

	//ManageAttack();
	
	update_timer.Start();

	return true;
}

bool j1AI::CleanUp()
{
	ai_worker->Reset();
	delete ai_worker;

	// Entities Data CleanUp
	ai_research_worker->HardReset();
	delete ai_research_worker;
	units_queue.clear();
	buildings_queue.clear();
	units_production.clear();
	buildings_production.clear();

	return true;
}

bool j1AI::LoadEnemies(const char * folder)
{
	//Load
	LOG("--- Loading %s...", folder);
	std::string load_folder = name + "/" + folder;
	pugi::xml_document enemy_data;

	if (!App->fs->LoadXML(load_folder.c_str(), &enemy_data))
	{
		LOG("Enemy AI load error!");
		return false;
	}

	pugi::xml_node unit_node = enemy_data.child("data").child("units").first_child();
	while (unit_node != NULL)
	{
		/*	Unit* new_unit = App->entities_manager->GenerateUnit(App->animator->StrToUnitEnum(unit_node.attribute("type").as_string()), ENEMY, false);
		new_unit->SetPosition(unit_node.attribute("pos_x").as_float(), unit_node.attribute("pos_y").as_float(), false);
		units_to_spawn.push_back(new_unit);
		unit_node = unit_node.next_sibling();*/


		//ai_worker->AddAICommand(new SpawnUnitCommand(App->animator->StrToUnitEnum(unit_node.attribute("type").as_string()), fPoint(unit_node.attribute("pos_x").as_float(0), unit_node.attribute("pos_y").as_float(0))));

		unit_node = unit_node.next_sibling();

	}
	return true;
}

Resource * j1AI::GetNearestNeed()
{
	RESOURCE_TYPE	type = BERRY_BUSH;
	uint			current_min = meat;

	if (wood < current_min)
	{
		current_min = wood;
		type		= TREE;
	}
	if(gold < current_min)
	{
		current_min = gold;
		type		= GOLD_ORE;
	}
	if (stone < current_min)
	{
		current_min = stone;
		type		= STONE_ORE;
	}

	return App->entities_manager->GetNearestResource(ai_starting_tc->GetPositionRounded(), type, NEUTRAL);;
}

void j1AI::ManageAttack()
{
	if (enemy_units.size() < 1) return;

	else 
	{
		std::list<Unit*>::const_iterator unit_it = enemy_units.begin();
		while (unit_it != enemy_units.end())
		{
			Building* to_kill = App->entities_manager->GetNearestBuilding(ai_starting_tc->GetPositionRounded(), TOWN_CENTER, ALLY);
			if (to_kill == nullptr) return;

			if (!unit_it._Ptr->_Myval->GetWorker()->IsBusy(SECONDARY))
				unit_it._Ptr->_Myval->GetWorker()->AddAction(App->action_manager->AttackToBuildingAction(unit_it._Ptr->_Myval, (Building*)to_kill), SECONDARY);

			unit_it++;
		}
	}
}

void j1AI::ManageConstrucion()
{

	BUILDING_TYPE new_building_type = GetNextBuildingSpawnType();

	GenerateBuilding(new_building_type);

}

void j1AI::ManageTroopsCreation()
{
	if (population >= max_population) return;

	UNIT_TYPE		unit_type		= UNIT_TYPE::NO_UNIT;

	GetSpawnUnitTypes(unit_type);

	GenerateUnit(unit_type);
}


// AI Entity Data Methods
void j1AI::LoadAIEntitiesData(pugi::xml_node& conf)
{
	next_research = 0;

	AI_Entities_Data_Buildings building_data;
	AI_Entities_Data new_data;
	for (int i = 0; i < 22; i++)
	{
		units_production.push_back(new_data);
		buildings_production.push_back(building_data);
	}

	pugi::xml_node item = conf.child("Buildings").first_child().first_child();
	building_data.spawn_from = VILLAGER;
	while (item != NULL)
	{
		building_data.base_type_building = App->animator->StrToBuildingEnum(item.attribute("base_type").as_string(""));
		building_data.cell_at_level = item.attribute("cl").as_int(0);
		building_data.cell_at_age = item.attribute("al").as_int(0);
		building_data.wood_cost = item.attribute("wc").as_int(0);
		building_data.food_cost = item.attribute("fc").as_int(0);
		building_data.gold_cost = item.attribute("gc").as_int(0);
		building_data.stone_cost = item.attribute("sc").as_int(0);
		building_data.population_cost = item.attribute("pc").as_int(0);
		building_data.b_type = (BUILDING_TYPE)item.attribute("bt").as_int(0);
		building_data.r_type = (RESEARCH_TECH)item.attribute("rt").as_int(0);
		buildings_queue.push_back(building_data);

		item = item.next_sibling();
	}
	UpdateAIBuildings();

	item = conf.child("Units").first_child();
	while (item != NULL)
	{
		new_data.base_type_unit = App->animator->StrToUnitEnum(item.attribute("base_type").as_string(""));
		new_data.spawn_from = App->animator->StrToBuildingEnum(item.attribute("spawn_from").as_string(""));
		new_data.spawn_from2 = App->animator->StrToBuildingEnum(item.attribute("spawn_from2").as_string(""));
		new_data.cell_at_level = item.attribute("cl").as_int(0);
		new_data.cell_at_age = item.attribute("al").as_int(0);
		new_data.wood_cost = item.attribute("wc").as_int(0);
		new_data.food_cost = item.attribute("fc").as_int(0);
		new_data.gold_cost = item.attribute("gc").as_int(0);
		new_data.stone_cost = item.attribute("sc").as_int(0);
		new_data.population_cost = item.attribute("pc").as_int(0);
		new_data.u_type = App->animator->StrToUnitEnum(item.attribute("ut").as_string(""));
		new_data.r_type = (RESEARCH_TECH)item.attribute("rt").as_int(0);

		units_queue.push_back(new_data);

		item = item.next_sibling();
	}
	UpdateAIUnits();

	item = conf.child("Research").first_child();
	while (item != NULL)
	{
		new_data.spawn_from = App->animator->StrToBuildingEnum(item.attribute("spawn_from").as_string(""));
		new_data.spawn_from2 = App->animator->StrToBuildingEnum(item.attribute("spawn_from2").as_string(""));
		new_data.cell_at_level = item.attribute("cl").as_int(0);
		new_data.cell_at_age = item.attribute("al").as_int(0);
		new_data.wood_cost = item.attribute("wc").as_int(0);
		new_data.food_cost = item.attribute("fc").as_int(0);
		new_data.gold_cost = item.attribute("gc").as_int(0);
		new_data.stone_cost = item.attribute("sc").as_int(0);
		new_data.population_cost = item.attribute("pc").as_int(0);
		new_data.r_type = (RESEARCH_TECH)item.attribute("rt").as_int(0);

		research_queue.push_back(new_data);

		item = item.next_sibling();
	}
	int x = 0;
}

void j1AI::UpgradeCivilization(RESEARCH_TECH type)
{
	next_research++;
	int size = 0;
	if (type == TC_CASTLE || type == TC_IMPERIAL)
	{
		current_age++;
		//update Available buildings
		bool update = false;
		size = buildings_production.size();
		for (int i = 0; i < size; i++)
		{
			if (buildings_production[i].r_type == type)
			{
				buildings_lvl[i]++;
				update = true;
			}
		}
		if (update) UpdateAIBuildings();
	}
	else
	{
		//Update Available Units;
		size = units_production.size();
		for (int i = 0; i < size; i++)
		{
			if (units_production[i].r_type == type)
			{
				units_lvl[i]++;
				UpdateAIUnits();
			}
		}
	}
}

void j1AI::UpdateResearch()
{
	if (research_timer.Read() > RESEARCH_RATE)
	{
		if (next_research < research_queue.size())
		{
			if (FindBuilding(research_queue[next_research].spawn_from, research_queue[next_research].spawn_from2) != nullptr)
			{
				if (CheckResources(research_queue[next_research].wood_cost, research_queue[next_research].food_cost, research_queue[next_research].gold_cost,
					research_queue[next_research].stone_cost, 0))
				{
					ai_research_worker->AddAction(App->action_manager->ResearchAction(research_queue[next_research].r_type, AI_RESEARCH_DURATION, ENEMY));
				}
			}
			else
			{
				if (GenerateBuilding(research_queue[next_research].spawn_from) != nullptr)
				{
					UpdateResearch();
					return;
				}
			}
		}
		research_timer.Start();
	}
}

void j1AI::UpdateAIUnits()
{
	// Set Current Cells
	int size = units_queue.size();
	for (int i = 0; i < size; i++)
	{
		if (units_queue[i].cell_at_level == units_lvl[units_queue[i].base_type_unit] && units_queue[i].cell_at_age <= current_age)
		{
			units_production[units_queue[i].base_type_unit] = units_queue[i];
		}
	}
}
void j1AI::UpdateAIBuildings()
{
	int size = buildings_queue.size();
	for (int i = 0; i < size; i++)
	{
		if (buildings_queue[i].cell_at_level == buildings_lvl[buildings_queue[i].base_type_building] && buildings_queue[i].cell_at_age <= current_age)
		{
			buildings_production[buildings_queue[i].base_type_building] = buildings_queue[i];
		}
	}
}

void j1AI::GetSpawnUnitTypes(UNIT_TYPE& u_type)
{
	uint troop_count = 0;
	UNIT_TYPE spawn_type = VILLAGER;

	std::list<Unit*>::const_iterator unit_it = enemy_units.begin();
	while (unit_it != enemy_units.end())
	{
		if (unit_it._Ptr->_Myval->GetUnitType() == spawn_type)
		{
			++troop_count;
			if (troop_count > MIN_VILLAGERS)
			{
				spawn_type = GetNextSpawnType(spawn_type);
			}
		}
		
		unit_it++;
	}
	
	u_type = spawn_type;


}

UNIT_TYPE j1AI::GetNextSpawnType(UNIT_TYPE u_type)
{
	UNIT_TYPE ret = UNIT_TYPE::NO_UNIT;

	switch (u_type)
	{
	case NO_UNIT:
		break;
	case MILITIA:
		ret = MILITIA;
		break;
	case ARBALEST:
		break;
	case ARCHER:
		break;
	case CAVALIER:
		break;
	case CAVALRY_ARCHER:
		break;
	case ELITE_SKIRMISHER:
		break;
	case HEAVY_CAVALRY_ARCHER:
		break;
	case KNIGHT:
		break;
	case MONK:
		break;
	case PALADIN:
		break;
	case PIKEMAN:
		break;
	case SPEARMAN:
		break;
	case VILLAGER:
		ret = MILITIA;
		break;
	case TWO_HANDED_SWORDMAN:
		break;
	case MAN_AT_ARMS:
		break;
	case LONG_SWORDMAN:
		break;
	case CHAMPION:
		break;
	case WARRIOR_CHMP:
		break;
	case ARCHER_CHMP:
		break;
	case WIZARD_CHMP:
		break;
	case CROSSBOWMAN:
		break;
	case SKIRMISHER:
		break;
	default:
		break;
	}

	return ret;
}

Building * j1AI::FindBuilding(BUILDING_TYPE type, BUILDING_TYPE type2)
{
	BUILDING_TYPE b_type;
	std::list<Building*>::const_iterator building = enemy_buildings.end();
	while (building != enemy_buildings.begin())
	{
		building--;
		b_type = building._Ptr->_Myval->GetBuildingType();
		if (b_type == type || b_type == type2)
		{
			return building._Ptr->_Myval;
		}
	}
	return nullptr;
}

BUILDING_TYPE j1AI::GetNextBuildingSpawnType()
{
	BUILDING_TYPE ret = HOUSE_A;

	uint town_center = 0;
	uint barrack = 0;
	uint archery = 0;
	uint stable = 0;
	uint house = 0;
	uint lumber_camp = 0;
	uint mining_camp = 0;
	uint monastery = 0;
	uint castle = 0;

	if (population >= max_population)
	{
		ret = HOUSE_A;
		return ret;
	}

	std::list<Building*>::const_iterator building_it = enemy_buildings.begin();

	while (building_it != enemy_buildings.end())
	{
		BUILDING_TYPE b_type = building_it._Ptr->_Myval->GetBuildingType();

		if (b_type == TOWN_CENTER || b_type == TOWN_CENTER_C)				town_center++;
		else if (b_type == BARRACK || b_type == BARRACK_C)					barrack++;
		else if (b_type == ARCHERY_RANGE || b_type == ARCHERY_RANGE_C)		archery++;
		else if (b_type == STABLE || b_type == STABLE_C)					stable++;
		else if (b_type == LUMBER_CAMP)										lumber_camp++;
		else if (b_type == MINING_CAMP)										mining_camp++;
		else if (b_type == MONASTERY)										monastery++;
		else if (b_type == CASTLE)											castle++;
		else if (b_type == HOUSE_A || b_type == HOUSE_AI || b_type == HOUSE_B ||
			b_type == HOUSE_BI || b_type == HOUSE_C || b_type == HOUSE_CI)	house++;

		building_it++;
	}

	uint min = house;
	if (min > barrack)
	{
		ret = BARRACK;
		min = barrack;
	}
	if (min > archery)
	{
		ret = ARCHERY_RANGE;
		min = archery;
	}
	if (min > stable)
	{
		ret = STABLE;
		min = stable;
	}
	if (min > lumber_camp)
	{
		ret = LUMBER_CAMP;
		min = lumber_camp;
	}
	if (min > mining_camp)
	{
		ret = MINING_CAMP;
		min = mining_camp;
	}
	if (min > monastery)
	{
		ret = MONASTERY;
		min = monastery;
	}
	if (min > castle)
	{
		ret = CASTLE;
		min = castle;
	}

	return ret;
}

void j1AI::GenerateDebugVillager()
{
	ai_worker->AddAICommand(new SpawnUnitCommand(VILLAGER, ai_starting_tc));
}

bool j1AI::GenerateUnit(UNIT_TYPE type)
{
	if (type == NO_UNIT) return false;
	if (CheckResources(units_production[type].wood_cost, units_production[type].food_cost, units_production[type].gold_cost, units_production[type].stone_cost, units_production[type].population_cost,UNIT , false))
	{
		Building* productive_building = nullptr;
		BUILDING_TYPE b_type;
		std::list<Building*>::const_iterator building = enemy_buildings.end();
		while (building != enemy_buildings.begin())
		{
			building--;
			b_type = building._Ptr->_Myval->GetBuildingType();
			if (b_type == units_production[type].spawn_from || b_type == units_production[type].spawn_from2)
			{
				if (productive_building != nullptr)
				{
					if (productive_building->GetWorker()->IsBusy(PRIMARY))
					{
						productive_building = building._Ptr->_Myval;
					}
				}
				else productive_building = building._Ptr->_Myval;
			}
		}
		if (productive_building != nullptr)
		{
			CheckResources(units_production[type].wood_cost, units_production[type].food_cost, units_production[type].gold_cost, units_production[type].stone_cost, units_production[type].population_cost, UNIT);
			productive_building->AddAction(App->action_manager->SpawnAction((ProductiveBuilding*)productive_building, units_production[type].u_type, ENEMY));
		}
	}
}

Building* j1AI::GenerateBuilding(BUILDING_TYPE type)
{
	iPoint position = { 0,0 };
	if (type == NO_BUILDING) return nullptr;
	if (CheckResources(buildings_production[type].wood_cost, buildings_production[type].food_cost, buildings_production[type].gold_cost, buildings_production[type].stone_cost, buildings_production[type].population_cost, BUILDING, false))
	{
		Unit* villager = nullptr;
		UNIT_TYPE u_type;
		std::list<Unit*>::const_iterator unit = enemy_units.end();
		while (unit != enemy_units.begin())
		{
			unit--;
			u_type = unit._Ptr->_Myval->GetUnitType();
			if (u_type == VILLAGER)
			{
				if (villager != nullptr)
				{
					if (villager->GetWorker()->IsBusy(PRIMARY))
					{
						villager = unit._Ptr->_Myval;
					}
				}
				else villager = unit._Ptr->_Myval;
			}
		}
		if (villager != nullptr)
		{
			CheckResources(buildings_production[type].wood_cost, buildings_production[type].food_cost, buildings_production[type].gold_cost, buildings_production[type].stone_cost, buildings_production[type].population_cost, BUILDING);
			position = villager->GetPositionRounded();
			Building* new_building = App->entities_manager->GenerateBuilding(buildings_production[type].b_type, ENEMY, true);
			if (new_building == nullptr) return nullptr;

			j1Timer save_alpha_timer;
			srand(time(NULL));
			float displacement = 100.0;
			new_building->OnlySetPosition(position.x, position.y);
			while (!new_building->CheckZone(position.x, position.y))
			{
				int direction = rand() % 4;
				switch (direction)
				{
				case 0:				//Decrease y
					position.y -= (displacement);
					break;
				case 1:				//Increase x
					position.x += (displacement);
					break;
				case 2:				//Increase y
					position.y += (displacement);
					break;
				case 3:				//Decrease x
					position.x -= (displacement);
					break;
				default:
					break;
				}

				new_building->OnlySetPosition(position.x, position.y);

				if (save_alpha_timer.Read() > 4)
				{
					App->entities_manager->DeleteEntity(new_building);
					return nullptr;
				}
			}
			new_building->SetPosition(position.x, position.y);
			enemy_buildings.push_back(new_building);
			return new_building;
		}
	}
	return nullptr;
}

void j1AI::AddResources(PLAYER_RESOURCES type, int value)
{
	int research_value = value / 2;
	int progresion_value = value / 2;

	switch (type)
	{
	case GP_WOOD:
		wood += progresion_value;
		research_wood += research_value;
		break;
	case GP_MEAT:
		meat += progresion_value;
		research_meat += research_value;
		break;
	case GP_GOLD:
		gold += progresion_value;
		research_gold += research_value;
		break;
	case GP_STONE:
		stone += progresion_value;
		research_stone += research_value;
		break;
	case GP_NO_RESOURCE:
		break;
	default:
		break;
	}

}

bool j1AI::CheckResources(int amount_wood, int amount_food, int amount_gold, int amount_stone, int used_population, ENTITY_TYPE type, bool use)
{
	if (type == ENTITY_TYPE::UNIT || type == ENTITY_TYPE::BUILDING)
	{
		if (wood - amount_wood >= 0 && meat - amount_food >= 0 && gold - amount_gold >= 0 && stone - amount_stone >= 0 && (population + used_population <= max_population || used_population < 0))
		{
			if (use)
			{
				if (amount_wood != 0) wood -= amount_wood;
				if (amount_food != 0) meat -= amount_food;
				if (amount_gold != 0) gold -= amount_gold;
				if (amount_stone != 0) stone -= amount_stone;
				if (used_population >= 0) population += used_population;
				else if (used_population < 0)
				{
					if (max_population - used_population < 200) max_population -= used_population;
					else max_population = 200;
				}
			}
			return true;
		}
	}

	else 
	{
		if (research_wood - amount_wood >= 0 && research_meat - amount_food >= 0 && research_gold - amount_gold >= 0 && research_stone - amount_stone >= 0)
		{
			if (use)
			{
				if (amount_wood != 0)  research_wood  -= amount_wood;
				if (amount_food != 0)  research_meat  -= amount_food;
				if (amount_gold != 0)  research_gold  -= amount_gold;
				if (amount_stone != 0) research_stone -= amount_stone;
			}
			return true;
		}
	}

	return false;
}

///AICommands managment---------------------------------------------------------------------------------------------
//AiWokerk definition--------------------
AIWorker::AIWorker()
{
}

AIWorker::~AIWorker()
{
}

void AIWorker::Update()
{
	if (current_command != nullptr)
	{
		if (current_command->Execute())
		{
			delete current_command;
			current_command = nullptr;
		}
	}
	else if (!command_queue.empty())
	{
		current_command = command_queue.front();
		command_queue.pop_front();
		//Delete command if it can't be activated
		if (!current_command->Activation())
		{
			delete current_command;
			current_command = nullptr;
		}
	}

}

void AIWorker::AddAICommand(AICommand* command)
{
	command_queue.push_back(command);
}

void AIWorker::Reset()
{
	delete current_command;
	current_command = nullptr;

	AICommand* to_erase;

	while (!command_queue.empty())
	{
		to_erase = command_queue.front();
		command_queue.pop_front();
		RELEASE(to_erase);
	}
}
//---------------------------------------

//WaitCommand definition-----------------
WaitAICommand::WaitAICommand(uint time) : time(time)
{
}

WaitAICommand::~WaitAICommand()
{
}

bool WaitAICommand::Activation()
{
	timer.Start();
	return true;
}

bool WaitAICommand::Execute()
{
	if (timer.Read() > time)
	{
		App->sound->PlayGUIAudio(PLAYER_UNIT_ALERT);

		return true;
	}

	return false;
}
//---------------------------------------

//Spawn all units in the waiting list----
SpawnUnitsFromListCommand::SpawnUnitsFromListCommand(std::list<Unit*>* to_spawn) : to_spawn(to_spawn)
{
}

SpawnUnitsFromListCommand::~SpawnUnitsFromListCommand()
{
}

bool SpawnUnitsFromListCommand::Execute()
{
	//Add the Loaded units to the game logic
	while (!to_spawn->empty())
	{
		current_spawn = to_spawn->back();
		to_spawn->pop_back();
		fPoint pos = current_spawn->GetPosition();
		/*	App->entities_manager->AddUnit((Unit*)current_spawn);
		current_spawn->SetPosition(pos.x, pos.y);*/

		App->AI->enemy_units.push_back(current_spawn);
	}

	//Add a passive scann action by default to all units
	uint size = App->AI->enemy_units.size();


	std::list<Unit*>::iterator it = App->AI->enemy_units.begin();
	while (it != App->AI->enemy_units.end())
	{
		it._Ptr->_Myval->AddAction(App->action_manager->ScanAction(it._Ptr->_Myval), TASK_CHANNELS::PASSIVE);

		it++;
	}



	return true;
}

//---------------------------------------


MoveUnitsCommand::MoveUnitsCommand(std::list<Entity*>& to_move_list, iPoint new_pos) : to_move_list(to_move_list), new_pos(new_pos)
{
}

MoveUnitsCommand::~MoveUnitsCommand()
{
}

bool MoveUnitsCommand::Execute()
{


	uint size = to_move_list.size();

	App->group_move->GetGroupOfUnits(&to_move_list, new_pos.x, new_pos.y, false);

	std::list<Unit*>::iterator it = App->AI->enemy_units.begin();


	return true;
}



//Spawn X unit-----------------
SpawnUnitCommand::SpawnUnitCommand(UNIT_TYPE type, ProductiveBuilding* generator) : type(type), generator(generator)
{


}

SpawnUnitCommand::~SpawnUnitCommand()
{
}

bool SpawnUnitCommand::Execute()
{
	//If AI already has max units end the spawn
	if (App->AI->population >= MAX_POPULATION) return true;

	generator->GetWorker()->AddAction(new SpawnUnitAction(generator, type, ENEMY));

	/*Unit* new_unit = App->entities_manager->GenerateUnit(type, ENEMY);
	new_unit->AddAction(App->action_manager->ScanAction(new_unit), TASK_CHANNELS::PASSIVE);
	App->AI->enemy_units.push_back(new_unit);
	*/

	return true;
}




//-----------------------------


//-----------------------------
SendToRecollect::SendToRecollect(std::list<Unit*> units, Resource* target) : units(units), target(target)
{
}

SendToRecollect::~SendToRecollect()
{
}

bool SendToRecollect::Execute()
{
	if (units.empty()) return true;
	//Iterate all enemy units to give orders
	std::list<Unit*>::const_iterator unit_it = units.begin();
	while (unit_it != units.end())
	{
		//Check resource type
		if (unit_it._Ptr->_Myval->GetUnitType() != VILLAGER)
		{
			unit_it++;
			continue;
		}

		if (!unit_it._Ptr->_Myval->GetWorker()->IsBusy(TASK_CHANNELS::SECONDARY))
		{
			unit_it._Ptr->_Myval->GetWorker()->AddAction(App->action_manager->RecollectAction((Villager*)unit_it._Ptr->_Myval, target), SECONDARY);
		}
		unit_it++;
	}

	return true;
}
//-----------------------------

///------------------------------------------------------------------------------------------------------------------


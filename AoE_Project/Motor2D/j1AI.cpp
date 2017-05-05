
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
	units_to_spawn.clear();
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
	

	/*	WIP, infinite loop possible
	if (building_timer.Read() > 1000)
	{
		ManageConstrucion();

		building_timer.Start();
	}
	*/

	//only update every 2 seconds
	if (update_timer.Read() < 10000)
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

	GenerateDebugVillager();
	
	//Enemies attack NOT WORKING
	
	std::list<Building*>::const_iterator building_it = enemy_buildings.begin();
	while (building_it != enemy_buildings.end())
	{
		if (building_it._Ptr->_Myval->GetBuildingType() == BARRACK)
		{
			building_it._Ptr->_Myval->GetWorker()->AddAction(App->action_manager->SpawnAction((ProductiveBuilding*)building_it._Ptr->_Myval, MILITIA, ENEMY));

		}
		building_it++;
	}
	ManageAttack();
	



	update_timer.Start();

	return true;
}

bool j1AI::CleanUp()
{
	ai_worker->Reset();
	delete ai_worker;

	ai_research_worker->HardReset();
	delete ai_research_worker;

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
	fPoint new_pos(0,0);
	float displacement = 100.0;
	
	Building* test = App->entities_manager->GenerateBuilding(BUILDING_TYPE::BARRACK, ENEMY);

	
	//
	srand(time(NULL));


	new_pos = ai_starting_tc->GetPosition();

	test->OnlySetPosition(new_pos.x, new_pos.y);


	while (!test->CheckZone(new_pos.x,new_pos.y))
	{
		int direction = rand() % 4;
		switch (direction)
		{
		case 0:				//Decrease y
			new_pos.y -= (displacement);
			break;
		case 1:				//Increase x
			new_pos.x += (displacement);
			break;
		case 2:				//Increase y
			new_pos.y += (displacement);
			break;
		case 3:				//Decrease x
			new_pos.x -= (displacement);
			break;
		default:
			break;
		}

		test->OnlySetPosition(new_pos.x, new_pos.y);

	}


	test->SetPosition(new_pos.x, new_pos.y);
}


// AI Entity Data Methods
void j1AI::LoadAIEntitiesData(pugi::xml_node& conf)
{
	next_research = 0;

	AI_Entities_Data new_data;
	pugi::xml_node item = conf.child("Research").first_child();
	while (item != NULL)
	{
		new_data.base_type = (UNIT_TYPE)item.attribute("base_type").as_int(0);
		new_data.cell_at_level = item.attribute("cl").as_int(0);
		new_data.cell_at_age = item.attribute("al").as_int(0);
		new_data.wood_cost = item.attribute("wc").as_int(0);
		new_data.food_cost = item.attribute("fc").as_int(0);
		new_data.gold_cost = item.attribute("gc").as_int(0);
		new_data.stone_cost = item.attribute("sc").as_int(0);
		new_data.population_cost = item.attribute("pc").as_int(0);

		new_data.u_type = (UNIT_TYPE)item.attribute("ut").as_int(0);
		new_data.b_type = (BUILDING_TYPE)item.attribute("bt").as_int(0);
		new_data.r_type = (RESEARCH_TECH)item.attribute("rt").as_int(0);

		research_queue.push_back(new_data);

		item = item.next_sibling();
	}
}

void j1AI::UpgradeCivilization(RESEARCH_TECH type)
{
	
}

void j1AI::UpdateResearch()
{
	if (research_timer.Read() > RESEARCH_RATE)
	{
		if (next_research < research_queue.size())
		{
			if (CheckResources(research_queue[next_research].wood_cost, research_queue[next_research].food_cost, research_queue[next_research].gold_cost,
				research_queue[next_research].stone_cost, 0))
			{
				ai_research_worker->AddAction(App->action_manager->ResearchAction(research_queue[next_research].r_type, AI_RESEARCH_DURATION, ENEMY));
				next_research++;
			}
		}
		research_timer.Start();
	}
}

void j1AI::GenerateDebugVillager()
{
	ai_worker->AddAICommand(new SpawnUnitCommand(VILLAGER, ai_starting_tc));

}

void j1AI::AddResources(PLAYER_RESOURCES type, int value)
{
	switch (type)
	{
	case GP_WOOD:
		wood += value;
		break;
	case GP_MEAT:
		meat += value;
		break;
	case GP_GOLD:
		gold += value;
		break;
	case GP_STONE:
		stone += value;
		break;
	case GP_NO_RESOURCE:
		break;
	default:
		break;
	}

}

bool j1AI::CheckResources(int amount_wood, int amount_food, int amount_gold, int amount_stone, int used_population, bool use)
{
	if (wood - amount_wood >= 0 && meat - amount_food >= 0 && gold - amount_gold >= 0 && stone - amount_stone >= 0 && (population + used_population <= max_population || used_population < 0))
	{
		if (use)
		{
			if (amount_wood != 0) wood -= amount_wood;
			if (amount_food != 0) meat -=amount_food;
			if (amount_gold != 0) gold -= amount_gold;
			if (amount_stone != 0) stone -= amount_stone;
			if (used_population >= 0) population += used_population;
			else if (used_population < 0)
			{
				if(max_population - used_population < 200) max_population -=used_population;
				else max_population = 200;
			}
		}
		return true;
	}
	else return false;
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


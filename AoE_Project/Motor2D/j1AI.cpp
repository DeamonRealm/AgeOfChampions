
#include "j1App.h"
#include "j1AI.h"
#include "j1EntitiesManager.h"
#include "j1FileSystem.h"
#include "j1Map.h"
#include "j1ActionManager.h"
#include "Actions_Unit.h"

j1AI::j1AI()
{
	name = "AI";
}

j1AI::~j1AI()
{
}

void j1AI::Enable()
{
	active = LoadEnemies("EnemiesSpawn.xml");


	ai_worker->AddAICommand(new WaitAICommand(5000));
	ai_worker->AddAICommand(new SpawnUnitsFromListCommand(&units_to_spawn));
	ai_worker->AddAICommand(new MoveUnitsCommand(enemy_units, App->map->MapToWorld(90,90)));


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

	ai_worker = new AIWorker;

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
	return true;
}

bool j1AI::CleanUp()
{
	ai_worker->Reset();
	delete ai_worker;

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
		Unit* new_unit = App->entities_manager->GenerateUnit(App->animator->StrToUnitEnum(unit_node.attribute("type").as_string()), ENEMY, false);
		new_unit->SetPosition(unit_node.attribute("pos_x").as_int(0), unit_node.attribute("pos_y").as_int(0), false);
		units_to_spawn.push_back(new_unit);
		unit_node = unit_node.next_sibling();
	}
	return true;
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
	if (timer.Read() > time)return true;
	

	return false;
}
//---------------------------------------

//Spawn all units in the waiting list----
SpawnUnitsFromListCommand::SpawnUnitsFromListCommand(std::vector<Unit*>* to_spawn) : to_spawn(to_spawn)
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
		App->entities_manager->AddUnit(current_spawn);

		App->AI->enemy_units.push_back(current_spawn);
	}

	//Add a passive scann action by default to all units
	uint size = App->AI->enemy_units.size();

	for (uint i = 0; i < size; i++)
	{
		App->AI->enemy_units[i]->AddPasiveAction(App->action_manager->ScanAction(App->AI->enemy_units[i]));
	};

	return true;
}

//---------------------------------------


MoveUnitsCommand::MoveUnitsCommand(std::vector<Unit*>& to_move_list, iPoint new_pos) : to_move_list(to_move_list), new_pos(new_pos)
{
}

MoveUnitsCommand::~MoveUnitsCommand()
{
}

bool MoveUnitsCommand::Execute()
{

	uint size = to_move_list.size();

	for (uint i = 0; i < size; i++)
	{
		to_move_list[i]->AddSecondaryAction(App->action_manager->MoveAction(to_move_list[i], new_pos));
	};

	return true;
}





///------------------------------------------------------------------------------------------------------------------
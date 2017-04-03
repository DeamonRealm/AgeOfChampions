#include "j1ActionManager.h"
#include "Actions_Unit.h"
#include "Actions_Building.h"


///Class Action ---------------------------------
//Base class to build action definitions
//Constructors ========================
Action::Action(Entity* actor, TASK_TYPE type) : actor(actor), type (type)
{

}

//Destructors =========================
Action::~Action()
{

}
TASK_TYPE Action::GetType()
{
	return type;
}
/// ---------------------------------------------



/// Action Manager ------------------------------
//Action Manager Module
//Constructors ========================
j1ActionManager::j1ActionManager()
{
	name = "action_manager";
}

//Destructors =========================
j1ActionManager::~j1ActionManager()
{

}

//Game Loop ===========================
bool j1ActionManager::CleanUp()
{

	//Freeing space of all actions created

	all_actions.clear();

	return true;
}

//Action Calls ========================
MoveUnitAction* j1ActionManager::MoveAction(Unit * actor, const iPoint& destination, const iPoint& target)
{
	//Generate a new move action definition
	MoveUnitAction* action = new MoveUnitAction(actor, destination, target);

	//Add the new action at the action manager
	all_actions.push_back(action);

	return action;
}

MoveUnitAction* j1ActionManager::MoveAction(std::vector<iPoint>* path, Unit * actor, const iPoint& target )
{
	//Generate a new move action definition
	MoveUnitAction* action = new MoveUnitAction(actor, path,target);

	//Add the new action at the action manager
	all_actions.push_back(action);

	return action;
}

AttackUnitAction* j1ActionManager::AttackToUnitAction(Unit * actor, Unit * target)
{
	//Generate a new attack action definition
	AttackUnitAction* action = new AttackUnitAction(actor, target);

	//Add the new action at the action manager
	all_actions.push_back(action);

	return action;
}

AttackBuildingAction * j1ActionManager::AttackToBuildingAction(Unit * actor, Building * target)
{
	//Generate a new move action definition
	AttackBuildingAction* action = new AttackBuildingAction(actor, target);

	//Add the new action at the action manager
	all_actions.push_back(action);

	return action;
}

DieUnitAction * j1ActionManager::DieAction(Unit * actor)
{
	//Generate a new die action definition
	DieUnitAction* action = new DieUnitAction(actor);

	//Add the new action at the action manager
	all_actions.push_back(action);

	return action;
}

StunUnitAction * j1ActionManager::StunAction(Unit * actor, uint time)
{
	//Generate a new move action definition
	StunUnitAction* action = new StunUnitAction(actor, time);

	//Add the new action at the action manager
	all_actions.push_back(action);

	return action;
}

RecollectVillagerAction* j1ActionManager::RecollectAction(Villager* actor, Resource** target)
{
	//Generate a new move action definition
	RecollectVillagerAction* action = new RecollectVillagerAction(actor, target);

	//Add the new action at the action manager
	all_actions.push_back(action);

	return action;
}

SaveResourcesVillagerAction * j1ActionManager::SaveResourcesAction(Villager * actor, Building * target)
{
	//Generate a new move action definition
	SaveResourcesVillagerAction* action = new SaveResourcesVillagerAction(actor, target);

	//Add the new action at the action manager

	all_actions.push_back(action);

	return action;
}

SpawnUnitAction* j1ActionManager::SpawnAction(ProductiveBuilding * actor, UNIT_TYPE type, DIPLOMACY diplomacy)
{
	SpawnUnitAction* action = new SpawnUnitAction(actor, type, diplomacy);

	all_actions.push_back(action);

	return action;
}

ScannAction * j1ActionManager::ScanAction(Entity * actor)
{
	ScannAction* action = new ScannAction((Unit*)actor);

	all_actions.push_back(action);

	return action;
}

/// ---------------------------------------------



///Action Worker---------------------------------
//Constructor
ActionWorker::ActionWorker() : refresh_rate(1000)
{
}

//Destructor
ActionWorker::~ActionWorker()
{
}

//Worker methods
void ActionWorker::Update()
{
	if (paused)
		return;
	
	//If the worker has a current action execute it

	DoWork(&action_queue, &current_action);

	//Make the passive_action logic only work on a refresh rate
	if (refresh_timer.Read() > refresh_rate)
	{
		DoWork(&passive_action_queue, &current_passive_action);
		refresh_timer.Start();
	}

}

void ActionWorker::DoWork(std::list<Action*>* queue, Action ** current)
{
	//If the worker has a current action execute it
	if (*current != nullptr)
	{
		//If the action ends (execute() == true) erase it
		if ((*current)->Execute())
		{
			delete (*current);
			(*current) = nullptr;
		}
	}
	//Else, If the worker has any action waiting mark the first one as current
	else if (!queue->empty())
	{
		(*current) = queue->front();
		queue->pop_front();

		//Delete action if it can't be activated
		if (!(*current)->Activation())
		{
			delete (*current);
			(*current) = nullptr;
		}
	}
}


void ActionWorker::AddAction(Action * action)
{
	action_queue.push_back(action);
}

void ActionWorker::AddPassiveAction(Action* action)
{
	passive_action_queue.push_back(action);
}

void ActionWorker::AddPriorizedAction(Action * action)
{
	if (current_action != nullptr)
	{
		action_queue.push_front(current_action);
		current_action = action;
		current_action->Activation();
	}
	else
	{
		current_action = action;
		current_action->Activation();
	}
}

void ActionWorker::PopAction(Action * action)
{
	if (current_action == action)current_action = nullptr;
	else
	{
		std::list<Action*>::iterator act = action_queue.begin();
		while (act != action_queue.end())
		{
			if (act._Ptr->_Myval == action)
			{
				action_queue.remove(action);
				return;
			}
			act++;
		}
	}
}

void ActionWorker::Reset()
{
	ResetQueue(&action_queue, &current_action);
	ResetQueue(&passive_action_queue, &current_passive_action);

	paused = false;
}

void ActionWorker::ResetActive()
{
	ResetQueue(&action_queue, &current_action);
}

void ActionWorker::ResetQueue(std::list<Action*>* queue, Action ** current)
{
	delete (*current);
	(*current) = nullptr;

	Action* to_erase;

	while (!queue->empty())
	{
		to_erase = queue->front();
		queue->pop_front();
		RELEASE(to_erase);
	}

	queue->clear();
}

TASK_TYPE ActionWorker::GetCurrentActionType() const
{
	if (current_action != nullptr)
	{
		return current_action->GetType();
	}

	return TASK_TYPE::TASK_NONE;
}

Action * ActionWorker::GetCurrentAction() const
{
	return current_action;
}

void ActionWorker::Pause()
{
	paused = true;
}
void ActionWorker::Restart()
{
	paused = false;
}
///----------------------------------------------
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
	actor = nullptr;
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

AttackUnitAction* j1ActionManager::AttackToUnitAction(Unit * actor, Unit ** target)
{
	//Generate a new attack action definition
	AttackUnitAction* action = new AttackUnitAction(actor, target);

	//Add the new action at the action manager
	all_actions.push_back(action);

	return action;
}

AttackBuildingAction * j1ActionManager::AttackToBuildingAction(Unit * actor, Building** target)
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

DieBuildingAction * j1ActionManager::DieBuildngAction(Building* actor)
{
	//Generate a new die action definition
	DieBuildingAction* action = new DieBuildingAction(actor);

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

ResearchTecAction * j1ActionManager::ResearchAction(RESEARCH_TECH type, uint r_time, j1Module * callback, DIPLOMACY diplomacy)
{
	ResearchTecAction* action = new ResearchTecAction(type, r_time, callback, diplomacy);

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
ActionWorker::ActionWorker() : refresh_rate(500)
{
}

//Destructor
ActionWorker::~ActionWorker()
{
	HardReset();
}

//Worker methods
void ActionWorker::Update()
{
	//Don't update if the worker is paused
	if (paused)
		return;
		
	//If the worker has a current action execute it
	if (DoWork(&primary_action_queue, &current_primary_action))
	{

		//Reeactive all actions form passive flows when any active action finshes
		if(current_secondary_action != nullptr)	current_secondary_action->Activation();
		if(current_passive_action != nullptr)	current_passive_action->Activation();
	}

	//Secondary and passive actions take place when there is no active_action
	if (current_primary_action == nullptr)
	{
		DoWork(&secondary_action_queue, &current_secondary_action);

		//Make the passive_action logic only work on a refresh rate
		if (refresh_timer.Read() > refresh_rate)
		{
			DoWork(&passive_action_queue, &current_passive_action);
			refresh_timer.Start();
		}
	}
}

bool ActionWorker::DoWork(std::list<Action*>* queue, Action ** current)
{
	//If the worker has a current action execute it
	if (*current != nullptr)
	{
		//If the action ends (execute() == true) erase it
		if ((*current)->Execute())
		{
			(*current)->CleanUp();
			delete (*current);
			(*current) = nullptr;
			return true;
		}
		return false;
	}
	//If the worker has any waiting actions mark the first one as current
	if (!queue->empty() && *current == nullptr)
	{
		(*current) = queue->front();
		queue->pop_front();

		//Delete action if it can't be activated
		if (!(*current)->Activation())
		{
			(*current)->CleanUp();
			delete (*current);
			(*current) = nullptr;
		}
	}
	return false;
}


void ActionWorker::AddAction(Action * action, TASK_CHANNELS channel)
{
	switch (channel)
	{
	case PRIMARY:
		primary_action_queue.push_back(action);
		break;
	case SECONDARY:
		secondary_action_queue.push_back(action);
		break;
	case PASSIVE:
		passive_action_queue.push_back(action);
		break;
	default:
		break;
	}

}



void ActionWorker::AddPriorizedAction(Action * action, TASK_CHANNELS channel)
{
	switch (channel)
	{
	case PRIMARY:
		AddPriorized(action, &primary_action_queue, &current_primary_action);
		break;
	case SECONDARY:
		AddPriorized(action, &secondary_action_queue, &current_secondary_action);
		break;
	case PASSIVE:
		AddPriorized(action, &passive_action_queue, &current_passive_action);
		break;
	default:
		break;
	}
}

void ActionWorker::AddPriorized(Action* action, std::list<Action*>* queue, Action** current)
{
	//Check if there is a current action if it is push it to the front of the queue
	if ((*current) != nullptr)	queue->push_front(*current);

	//Make the new action the current action and activate it
	(*current) = action;
	(*current)->Activation();
}

void ActionWorker::PopAction(Action * action)
{
	if (current_primary_action == action)current_primary_action = nullptr;
	else
	{
		std::list<Action*>::iterator act = primary_action_queue.begin();
		while (act != primary_action_queue.end())
		{
			if (act._Ptr->_Myval == action)
			{
				primary_action_queue.remove(action);
				return;
			}
			act++;
		}
	}
}

void ActionWorker::HardReset()
{
	ResetQueue(&primary_action_queue,	&current_primary_action);
	ResetQueue(&secondary_action_queue, &current_secondary_action);
	ResetQueue(&passive_action_queue,	&current_passive_action);

	paused = false;
}

void ActionWorker::ResetChannel(TASK_CHANNELS channel)
{
	switch (channel)
	{
	case PRIMARY:
		ResetQueue(&primary_action_queue, &current_primary_action);
		break;
	case SECONDARY:
		ResetQueue(&secondary_action_queue, &current_secondary_action);
		break;
	case PASSIVE:
		ResetQueue(&passive_action_queue, &current_passive_action);
		break;
	default:
		break;
	}

}


void ActionWorker::ResetQueue(std::list<Action*>* queue, Action ** current)
{
	if ((*current) != nullptr)
	{
		(*current)->CleanUp();
		delete (*current);
		(*current) = nullptr;
	}

	Action* to_erase;

	while (!queue->empty())
	{
		to_erase = queue->front();
		to_erase->CleanUp();
		queue->pop_front();
		RELEASE(to_erase);
	}

	queue->clear();
}

TASK_TYPE ActionWorker::GetCurrentActionType() const
{
	if (current_primary_action != nullptr)
	{
		return current_primary_action->GetType();
	}

	return TASK_TYPE::TASK_NONE;
}

Action * ActionWorker::GetCurrentAction() const
{
	return current_primary_action;
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
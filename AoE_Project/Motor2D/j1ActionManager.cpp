#include "j1ActionManager.h"
#include "Actions_Unit.h"

///Class Action ---------------------------------
//Base class to build action definitions
//Constructors ========================
Action::Action(Entity* actor) :actor(actor)
{

}

//Destructors =========================
Action::~Action()
{

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
	/*std::list<Action*>::iterator action_item = all_actions.begin();
	while (action_item != all_actions.end())
	{
		RELEASE(action_item._Ptr->_Myval);
		action_item++;
	}*/
	all_actions.clear(); /*Actions are deleted by the worker so this don't make sense*/

	return false;
}

//Action Calls ========================
MoveUnitAction* j1ActionManager::MoveAction(Unit * actor, int x, int y)
{
	//Generate a new move action definition
	MoveUnitAction* action = new MoveUnitAction(actor, x, y);
	
	//Add the new action at the action manager
	all_actions.push_back(action);

	return action;
}

AttackUnitAction* j1ActionManager::AttackToUnitAction(Unit * actor, Unit * target)
{
	//Generate a new move action definition
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
	//Generate a new move action definition
	DieUnitAction* action = new DieUnitAction(actor);

	//Add the new action at the action manager
	all_actions.push_back(action);

	return action;
}
/// ---------------------------------------------



///Action Worker---------------------------------
//Construcor
ActionWorker::ActionWorker()
{
}

//Destructor
ActionWorker::~ActionWorker()
{
}

//Worker methods
void ActionWorker::Update()
{
	if (current_action != nullptr)
	{
		if (current_action->execute())
		{
			delete current_action;
			current_action = nullptr;
		}
	}
	else if (!action_queue.empty())
	{
		current_action = action_queue.front();
		action_queue.pop();
	}

}
void ActionWorker::AddAction(Action * action)
{
	action_queue.push(action);
}
///----------------------------------------------
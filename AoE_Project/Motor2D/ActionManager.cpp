#include "ActionManager.h"

//Action class---------------------
Action::Action()
{
	App->entities_manager->entity_actions.push_back(this);
}
Action::~Action()
{
}
//---------------------------------



//Action Manager

ActionManager::ActionManager()
{

}

ActionManager::~ActionManager()
{
}

void ActionManager::Add(Action * new_action)
{
	all_actions.push_back(new_action);
}

void ActionManager::CleanUp()
{
	//Clean Up all unit actions;
	std::list<Action*>::iterator action_item = all_actions.begin();
	while (action_item != all_actions.end())
	{
		RELEASE(action_item._Ptr->_Myval);
		action_item++;
	}
}


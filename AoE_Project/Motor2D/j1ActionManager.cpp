#include "j1ActionManager.h"

///Class Action ---------------------------------
//Base class to build action definitions
//Constructors ========================
Action::Action(Entity* actor) :actor(actor)
{
	//Added to Manager all list to be safely erased later
	App->action_manager->all_actions.push_back(this);	
}

//Destructors =========================
Action::~Action()
{

}
/// ---------------------------------------------



//Action Manager

j1ActionManager::j1ActionManager()
{
	name = "action_manager";
}

j1ActionManager::~j1ActionManager()
{
}

bool j1ActionManager::CleanUp()
{

	//Freeing space of all actions created
	std::list<Action*>::iterator action_item = all_actions.begin();
	while (action_item != all_actions.end())
	{
		RELEASE(action_item._Ptr->_Myval);
		action_item++;
	}
	all_actions.clear();

	return false;
}

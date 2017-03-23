#ifndef __ACTION_MANAGER_H__
#define __ACTION_MANAGER_H__

#include "j1App.h"
#include "j1EntitiesManager.h"

//Action virtual class (are commands but not for the console)
class Action
{
public:
	//Set all the Actions to a list on the Entities manager to clean them up at closing the app.
	Action();

	~Action();
public:
	//This function defines the action taking place
	virtual void execute() {}
	virtual bool IsDone() { return true; };

	///Each diferent action inheriting from this class should have custom
	///properties to do its actions.
};




//Action management class
///Should act like a list, but with simplified methods
class ActionManager
{
public:
	ActionManager();
	~ActionManager();

	void Add(Action* new_action);

	//Delete all actions in the list
	void CleanUp();

private:
	std::list<Action*> all_actions;
};



#endif // !__ACTION_MANAGER_H__


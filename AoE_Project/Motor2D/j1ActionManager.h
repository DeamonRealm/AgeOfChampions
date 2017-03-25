#ifndef __J1ACTION_MANAGER_H__
#define __J1ACTION_MANAGER_H__

#include "j1App.h"
#include "j1Module.h"
//#include "j1EntitiesManager.h"

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




//Action Manager Module
class j1ActionManager : public j1Module
{
public:
	j1ActionManager();
	~j1ActionManager();


	// Called before quitting
	bool CleanUp();

public:
	//List of all action JUST for CLEANUP 
	std::list<Action*> all_actions;

};



#endif // !__ACTION_MANAGER_H__


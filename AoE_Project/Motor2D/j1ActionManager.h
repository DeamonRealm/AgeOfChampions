#ifndef __J1ACTION_MANAGER_H__
#define __J1ACTION_MANAGER_H__

#include "j1Module.h"

class Entity;
class Unit;
class Resource;

class MoveUnitAction;

/// Class Action --------------------------------
//Action virtual class (are commands but not for the console)
class Action
{
public:

	//Set all the Actions to a list on the Entities manager to clean them up at closing the app.
	Action(Entity* actor);
	~Action();

public:

	Entity* actor = nullptr;

public:

	//This function defines the action taking place
	virtual void execute() {}
	virtual bool IsDone() { return true; }
	///Each different action inheriting from this class should have custom
	///properties to do its actions.

};
/// ---------------------------------------------


/// Action Manager ------------------------------
//Action Manager Module
class j1ActionManager : public j1Module
{
public:

	j1ActionManager();
	~j1ActionManager();

	// Called before quitting
	bool CleanUp();

public:

	//Action Calls --------------------
	MoveUnitAction*	MoveAction(Unit* actor, int x, int y);
	bool	AttackAction(Entity* target);
	bool	RecollectAction(Resource* target);

private:

	//List of all action JUST for CLEANUP 
	std::list<Action*> all_actions;

};
/// ---------------------------------------------
#endif // __ACTION_MANAGER_H__


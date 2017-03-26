#ifndef __J1ACTION_MANAGER_H__
#define __J1ACTION_MANAGER_H__

#include "j1Module.h"
#include <queue>

class Entity;
class Unit;
class Resource;
class Building;
class Villager;

class MoveUnitAction;
class AttackUnitAction;
class AttackBuildingAction;
class AttackResourceAction;
class DieUnitAction;
class RecollectVillagerAction;

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
	bool completed = false;

public:

	//This function defines the action taking place
	//Returns TRUE when execute is finished
	virtual bool execute()	{ return true; }
	virtual bool IsDone()	{ return true; }
	///Each different action inheriting from this class should have custom
	///properties to do its actions.

};
/// ---------------------------------------------


///Action Worker---------------------------------
class ActionWorker
{
public:

	ActionWorker();
	~ActionWorker();

private:

	std::queue<Action*> action_queue;
	Action* current_action = nullptr;

public:

	void Update();
	void AddAction(Action* action);
	void Reset();

};
///----------------------------------------------

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
	MoveUnitAction*				MoveAction(Unit* actor, int x, int y);
	AttackUnitAction*			AttackToUnitAction(Unit* actor, Unit *target);
	AttackBuildingAction*		AttackToBuildingAction(Unit* actor, Building* target);
	DieUnitAction*				DieAction(Unit* actor);
	RecollectVillagerAction*	RecollectAction(Villager* actor, Resource* target);

	/*AttackResourceAction*	AttackToResourceAction(Unit* actor, Resource* target);
	bool	RecollectAction(Resource* target);*/

private:

	//List of all action JUST for CLEANUP 
	std::list<Action*> all_actions;

};
/// ---------------------------------------------
#endif // __ACTION_MANAGER_H__


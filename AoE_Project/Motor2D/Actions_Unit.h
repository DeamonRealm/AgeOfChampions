#ifndef __ACTION_UNIT_H__
#define __ACTION_UNIT_H__

#include "j1ActionManager.h"
#include "BaseEntities.h"
#include "j1App.h"
#include "j1EntitiesManager.h"

//Needed to have the MoveUnitAction with the path
#include "j1Pathfinding.h"

/// Move Unit Action ----------------------------
//Move the character
class MoveUnitAction : public Action
{
public:

	//Constructor -----------
	MoveUnitAction(Unit* actor, int x, int y) : Action(actor), x_new(x), y_new(y)
	{
		//Set path so the unit starts moving
		//App->entities_manager->SetUnitPath((Unit*)actor, iPoint(x_new, y_new));

		iPoint origin(actor->GetPosition().x, actor->GetPosition().y);
		path = App->pathfinding->SimpleAstar(origin, iPoint(x_new, y_new));
	}

	//Functionality ---------
	bool execute()
	{
		return ((Unit*)actor)->Move(path);
	}

private:

	std::vector<iPoint>* path = nullptr;
	int x_new = 0;
	int y_new = 0;

};
/// ---------------------------------------------

/// Attack Unit Action ----------------------------
//Move the character
class AttackUnitAction : public Action
{
public:

	//Constructor -----------
	AttackUnitAction(Unit* actor,Unit* target) : Action(actor), target(target)
	{
		//Set actor interaction target
		((Unit*)actor)->SetInteractionTarget(target);
	}

	//Functionality ---------
	bool execute()
	{
		//Actor attack the target
		return ((Unit*)actor)->AttackUnit();
	}

private:

	Entity* target = nullptr;

};
/// ---------------------------------------------

/// Attack Building Action ----------------------------
//Move the character
class AttackBuildingAction : public Action
{
public:

	//Constructor -----------
	AttackBuildingAction(Unit* actor, Building* target) : Action(actor), target(target)
	{
		//Set actor interaction target
		((Unit*)actor)->SetInteractionTarget(target);
	}

	//Functionality ---------
	bool execute()
	{
		//Actor attack the target
		return ((Unit*)actor)->AttackBuilding();
	}

private:

	Building* target = nullptr;

};
/// ---------------------------------------------

/// Die Action ----------------------------
//Move the character
class DieUnitAction : public Action
{
public:

	//Constructor -----------
	DieUnitAction(Unit* actor) : Action(actor)
	{

	}

	//Functionality ---------
	bool execute()
	{
		//Actor attack the target
		return ((Unit*)actor)->Die();
	}
};
/// ---------------------------------------------

/// Recollect Action ----------------------------
//Recollect resources
class RecollectVillagerAction : public Action
{
public:

	//Constructor -----------
	RecollectVillagerAction(Villager* actor, Resource* target) : Action(actor), target(target)
	{
		//Set actor interaction target
		((Unit*)actor)->SetInteractionTarget(target);
	}

	//Functionality ---------
	bool execute()
	{
		//Actor recollect
		((Unit*)actor)->SetInteractionTarget(target);
		return ((Villager*)actor)->Recollect();
	}

private:

	Resource* target = nullptr;

};
/// ---------------------------------------------

/// Recollect Action ----------------------------
//Save the unit resources in a building
class SaveResourcesVillagerAction : public Action
{
public:

	//Constructor -----------
	SaveResourcesVillagerAction(Villager* actor, Building* target) : Action(actor), target(target)
	{
		//Set actor interaction target
		((Unit*)actor)->SetInteractionTarget(target);
	}

	//Functionality ---------
	bool execute()
	{
		//Actor save resources
		return ((Villager*)actor)->SaveResources();
	}

private:

	Building* target = nullptr;

};
/// ---------------------------------------------
#endif // !__ACTION_UNIT_H__


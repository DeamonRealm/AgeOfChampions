#ifndef __ACTION_UNIT_H__
#define __ACTION_UNIT_H__

#include "j1ActionManager.h"
#include "BaseEntities.h"
#include "j1App.h"
#include "j1EntitiesManager.h"
#include "j1SoundManager.h"

//Needed to have the MoveUnitAction with the path
#include "j1Pathfinding.h"

/// Move Unit Action ----------------------------
//Move the character
class MoveUnitAction : public Action
{
public:

	//Constructor -----------
	MoveUnitAction(Unit* actor, int x, int y) : Action(actor, TASK_U_MOVE), x_new(x), y_new(y)
	{

	}

	MoveUnitAction(Unit* actor, std::vector<iPoint>* path) : Action(actor, TASK_U_MOVE), path(path)
	{

	}

	//Functionality ---------
	bool Activation()
	{
		if (path == nullptr)
		{
			//Calculate the path
			iPoint origin(actor->GetPosition().x, actor->GetPosition().y);
			path = App->pathfinding->SimpleAstar(origin, iPoint(x_new, y_new));
			if (path == nullptr)return false;
		}
		((Unit*)actor)->SetAction(WALK);
		((Unit*)actor)->Focus(path->back());
		((Unit*)actor)->SetFutureAction(*(path->rbegin()+1));

		return true;
	}

	bool Execute()
	{
		return ((Unit*)actor)->Move(path);
	}

	//Returns the path 
	std::vector<iPoint>* GetPath() const
	{
		return path;
	};

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
	AttackUnitAction(Unit* actor,Unit* target) : Action(actor, TASK_U_ATTACK_U), target(target)
	{
		//Set actor interaction target
		((Unit*)actor)->SetInteractionTarget(target);
	}

	//Functionality ---------
	bool Execute()
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
	AttackBuildingAction(Unit* actor, Building* target) : Action(actor, TASK_U_ATTACK_B), target(target)
	{
		//Set actor interaction target
		((Unit*)actor)->SetInteractionTarget(target);
	}

	//Functionality ---------
	bool Execute()
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
	DieUnitAction(Unit* actor) : Action(actor, TASK_U_DIE)
	{
	}

	//Functionality ---------
	bool Execute()
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
	RecollectVillagerAction(Villager* actor, Resource* target) : Action(actor, TASK_U_RECOLLECT), target(target)
	{
	}

	//Functionality ---------
	bool Activation()
	{
		if (target == nullptr)return false;

		//Set actor interaction target
		((Unit*)actor)->SetInteractionTarget(target);

		//Set actor animation
		((Villager*)actor)->CheckRecollectAnimation(((Resource*)target)->GetResourceType());
		((Villager*)actor)->Focus(target->GetPositionRounded());

		return true;
	}

	bool Execute()
	{
		//Actor recollect
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
	SaveResourcesVillagerAction(Villager* actor, Building* target) : Action(actor,TASK_U_SAVE_RESOURCES), target(target)
	{
	}

	//Functionality ---------
	bool Activation()
	{
		//Set actor interaction target
		if (target != nullptr)
		{
			((Unit*)actor)->SetInteractionTarget(target);
			return true;
		}
		else
			return false;
	}

	bool Execute()
	{
		//Actor save resources
		return ((Villager*)actor)->SaveResources();
	}

private:

	Building* target = nullptr;

};
/// ---------------------------------------------
#endif // !__ACTION_UNIT_H__


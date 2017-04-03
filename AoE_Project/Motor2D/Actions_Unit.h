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
	MoveUnitAction(Unit* actor, const iPoint& destination, const iPoint& target) : Action(actor, TASK_U_MOVE), destination(destination), target(target)
	{
	}

	MoveUnitAction(Unit* actor, std::vector<iPoint>* path, const iPoint& target) : Action(actor, TASK_U_MOVE), path(path),target(target)
	{
	}

	//Functionality ---------
	bool Activation()
	{
		if (path == nullptr)
		{
			//Calculate the path
			iPoint origin(actor->GetPosition().x, actor->GetPosition().y);
			path = App->pathfinding->SimpleAstar(origin, destination);
			if (path == nullptr)return false;
		}
		((Unit*)actor)->SetAction(WALK);
		((Unit*)actor)->Focus(path->back(),true);
		if (path->size() > 2)	((Unit*)actor)->SetFutureAction(*(path->rbegin() + 1));
		else					((Unit*)actor)->SetFutureAction(iPoint(-1, -1));

		return true;
	}

	bool Execute()
	{
		return ((Unit*)actor)->Move(path,target);
	}

	//Returns the path 
	std::vector<iPoint>* GetPath() const
	{
		return path;
	};

private:

	std::vector<iPoint>* path = nullptr;

	iPoint destination;
	iPoint target;
};
/// ---------------------------------------------

/// Attack Unit Action ----------------------------
//Move the character
class AttackUnitAction : public Action
{
public:

	//Constructor -----------
	AttackUnitAction(Unit* actor,Unit** target) : Action(actor, TASK_U_ATTACK_U), target(target)
	{
		//Set actor interaction target
	//	((Unit*)actor)->SetInteractionTarget(target);
	}

	//Functionality ---------
	bool Execute()
	{
		if ((*target) == nullptr) return true;

		//Actor attack the target
		return ((Unit*)actor)->AttackUnit(target);
	}

private:

	Unit** target = nullptr;
};
/// ---------------------------------------------

/// Attack Building Action ----------------------------
//Move the character
class AttackBuildingAction : public Action
{
public:

	//Constructor -----------
	AttackBuildingAction(Unit* actor, Building** target) : Action(actor, TASK_U_ATTACK_B), target(target)
	{

	}

	//Functionality ---------
	bool Execute()
	{
		//Actor attack the target
		return ((Unit*)actor)->AttackBuilding(target);
	}

private:

	Building** target = nullptr;
};
/// ---------------------------------------------

/// Die Action ----------------------------
//Kill the character
class DieUnitAction : public Action
{
public:

	//Constructor -----------
	DieUnitAction(Unit* actor) : Action(actor, TASK_U_DIE)
	{
	}

	//Functionality ---------
	bool Activation() 
	{
		App->sound->PlayFXAudio(DEATH_SOUND);
		return true;
	}

	bool Execute()
	{
		//Actor attack the target
		return ((Unit*)actor)->Die();
	}
};
/// ---------------------------------------------

/// Die Building Action ----------------------------
//Kill the character
class DieBuildingAction : public Action
{
public:

	//Constructor -----------
	DieBuildingAction(Building* actor) : Action(actor, TASK_B_DIE)
	{
	}

	bool Execute()
	{
		//Actor attack the target
		return ((Building*)actor)->Die();
	}
};
/// ---------------------------------------------

/// Stun Action ----------------------------
//Stun the character
class StunUnitAction : public Action
{
public:

	//Constructor -----------
	StunUnitAction(Unit* actor, uint time) : Action(actor, TASK_U_STUN), time(time)
	{
	}

	//Functionality ---------
	bool Activation()
	{
		timer.Start();
		((Unit*)actor)->SetAction(IDLE);
		if (((Unit*)actor)->GetUnitType() == VILLAGER)
		{
			if (((Villager*)actor)->GetItemType() == GOLD || ((Villager*)actor)->GetItemType() == MEAT || ((Villager*)actor)->GetItemType() == STONE || ((Villager*)actor)->GetItemType() == WOOD)
			{
				((Villager*)actor)->SetItemType(NO_ITEM);
			}
		}
		App->animator->UnitPlay((Unit*)actor);

		return true;
	}

	//Functionality ---------
	bool Execute()
	{
		//Actor attack the target
		return timer.Read() > time;
	}

private:

	j1Timer timer;
	uint time = 0;

};
/// ---------------------------------------------

/// Recollect Action ----------------------------
//Recollect resources
class RecollectVillagerAction : public Action
{
public:

	//Constructor -----------
	RecollectVillagerAction(Villager* actor, Resource** target) : Action(actor, TASK_U_RECOLLECT), target(target)
	{
	}
	
	//Functionality ---------
	bool Activation()
	{
		//Check it the resource hasn't been yet depleted
		if (*target == nullptr)return false;

		//Set actor animation
		((Villager*)actor)->Focus((*target)->GetPositionRounded(), true);
		((Villager*)actor)->CheckRecollectResource(((Resource*)(*target))->GetResourceType());

		return true;
	}

	bool Execute()
	{
		if (*target == nullptr)
		{
			//If resource is deplated execute a SaveResourceAction
			Building* save_point = App->entities_manager->SearchNearestSavePoint(actor->GetPositionRounded());
			if (save_point != nullptr)
			{
				((Villager*)actor)->AddAction((Action*)App->action_manager->SaveResourcesAction((Villager*)actor, save_point));
			}

			//Reset the animation
			App->animator->UnitPlay((Villager*)actor);
			return true;
		}
		//Actor recollect
		return ((Villager*)actor)->Recollect(target);
	}

	
private:

	Resource** target = nullptr;

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
		if (target == nullptr)
		{
			((Villager*)actor)->ResetResourcesData();
			App->animator->UnitPlay((Villager*)actor);
			return false;
		}

		((Unit*)actor)->SetInteractionTarget(target);
		((Villager*)actor)->CheckCarryResource();
		return true;
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


///Scann for units action------------------------
class ScannAction : public Action
{
public:
	ScannAction(Unit* actor) : Action(actor, TASK_U_SCANN)
	{
	};
	~ScannAction() {};

public:

	bool Execute()
	{
		if (actor->GetWorker()->GetCurrentActionType() == TASK_U_DIE) return true;

		surrounding_units.clear();

		App->entities_manager->units_quadtree.CollectCandidates(surrounding_units, actor->GetVision());

		uint size = surrounding_units.size();

			for (uint i = 0; i < size; i++)
			{
				if (actor->GetDiplomacy() != surrounding_units[i]->GetDiplomacy())
				{
					actor->AddPriorizedAction(App->action_manager->AttackToUnitAction((Unit*)actor, (Unit**)surrounding_units[i]->GetMe()));
					return false;
				}
			};

		return false;
	};


private:
	std::vector<Unit*> surrounding_units;

};




///----------------------------------------------
#endif // !__ACTION_UNIT_H__


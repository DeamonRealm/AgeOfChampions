#ifndef __ACTION_UNIT_H__
#define __ACTION_UNIT_H__

#include "j1ActionManager.h"
#include "BaseEntities.h"
#include "j1App.h"
#include "j1EntitiesManager.h"
#include "j1SoundManager.h"
#include "j1Render.h"

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

	MoveUnitAction(Unit* actor, std::vector<iPoint>* path, const iPoint& target) : Action(actor, TASK_U_MOVE), path(path), target(target)
	{
		if (path != nullptr )
		{
			if (!path->empty())
				destination = path->front();
		}
	}

	//Destructor ------------
	~MoveUnitAction()
	{

	}

	//Functionality ---------
	bool Activation()
	{

		//Calculate the path
		iPoint origin(actor->GetPosition().x, actor->GetPosition().y);

		//Try to make on path if the action hasn't one
		if (path == nullptr)
		{
			path = App->pathfinding->SimpleAstar(origin, destination);
		}
		else
		{
			//Check if the unit is too far awasy from the start of the stored path and
			//repath if the total distance is higher than 500->~3 tiles
			bool repath_needed = abs(origin.DistanceOctile(path->back())) > 500;
			
			if (repath_needed)
			{
				path->clear();
				delete path;
				path = nullptr;
				
				path = App->pathfinding->SimpleAstar(origin, destination);
			}
		}
		

		//Chacked here so check both in case of repath and in case of not repath
		if (path == nullptr)	return false;
		
		//Animate the actor
		((Unit*)actor)->SetAction(WALK);
		((Unit*)actor)->Focus(path->back(), true);
		if (path->size() > 2)	((Unit*)actor)->SetFutureAction(*(path->rbegin() + 1));
		else					((Unit*)actor)->SetFutureAction(iPoint(-1, -1));

		return true;
	}

	bool Execute()
	{
		if (((Unit*)actor)->Move(path, target))
		{
			//Stop idle walk animation
			bool exception = false;
			ITEM_TYPE item_type;
			((Unit*)actor)->SetAction(IDLE);
			if (((Unit*)actor)->GetUnitType() == VILLAGER) {
				item_type = ((Villager*)actor)->GetItemType();
				if (item_type != ITEM_TYPE::NO_ITEM)
				{
					((Villager*)actor)->SetItemType(NO_ITEM);
					exception = true;
				}
			}
			App->animator->UnitPlay((Unit*)actor);
			if (exception)
			{
				((Villager*)actor)->SetItemType(item_type);
			}
			return true;
		}
		return false;
	}


	void CleanUp()
	{
		this->actor = nullptr;
		if (path != nullptr)
		{
			path->clear();
			delete path;
			path = nullptr;
		}
	}

	//Get Methods ------
	std::vector<iPoint>* GetPath() const
	{
		return path;
	};

	iPoint GetGoal()const
	{
		return destination;
	}

private:

	std::vector<iPoint>* path = nullptr;

	iPoint destination;
	iPoint target;
};
/// ---------------------------------------------

/// Teleport Unit Action ----------------------------
//Teleport the character
class TeleportUnitAction : public Action
{
public:
	//Constructor -----------
	TeleportUnitAction(Unit* actor, const iPoint& displacement) : Action(actor, TASK_U_TELEPORT), displacement(displacement)
	{
	}
	//Destructor ------------
	~TeleportUnitAction()
	{
	}



	bool Execute()
	{
		float or_x = actor->GetPosition().x;
		float or_y = actor->GetPosition().y;
		actor->SetPosition(or_x + displacement.x, or_y + displacement.y);

		return true;
	}


	void CleanUp()
	{
		this->actor = nullptr;
	}

	//Get Methods ------
	iPoint GetDisplacement()const
	{
		return displacement;
	}

private:

	iPoint displacement;

};
/// ---------------------------------------------

/// Attack Unit Action ----------------------------
//Move the character
class AttackUnitAction : public Action
{
public:

	//Constructor -----------
	AttackUnitAction(Unit* actor, Unit* target) : Action(actor, TASK_U_ATTACK_U), target(target)
	{
		//Set actor interaction target
		//	((Unit*)actor)->SetInteractionTarget(target);
	}

	//Destructor ------------
	~AttackUnitAction()
	{
		this->actor = nullptr;
		this->target = nullptr;
	}

	//Functionality ---------
	bool Execute()
	{
		if (target->GetLife() == 0)
		{
			((Unit*)actor)->SetAction(IDLE);
			App->animator->UnitPlay((Unit*)actor);
		}

		//Actor attack the target
		return ((Unit*)actor)->AttackUnit(target);
	}

	//Get Methods -----------
	Unit*	GetTarget()const
	{
		return target;
	}

private:

	Unit* target = nullptr;
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

	}

	//Destructor ------------
	~AttackBuildingAction()
	{
		this->actor = nullptr;
		this->target = nullptr;
	}

	//Functionality ---------
	bool Execute()
	{
		if (target == nullptr || actor == nullptr) return true;
		if (target->GetLife() == 0)
		{
			((Unit*)actor)->SetAction(IDLE);
			App->animator->UnitPlay((Unit*)actor);
		}

		//Actor attack the target
		return ((Unit*)actor)->AttackBuilding(target);
	}

	//Get Methods -----------
	Building*	GetTarget()const
	{
		return target;
	}

private:

	Building* target = nullptr;
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
		if (App->render->IsOnCamera(actor->GetPositionRounded()))
		{
			App->sound->PlayFXAudio(DEATH_SOUND);
		}

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
		((Unit*)actor)->SetAction(IDLE);
		if (((Unit*)actor)->GetUnitType() == VILLAGER)
		{
			if (((Villager*)actor)->GetItemType() == GOLD || ((Villager*)actor)->GetItemType() == MEAT || ((Villager*)actor)->GetItemType() == STONE || ((Villager*)actor)->GetItemType() == WOOD)
			{
				((Villager*)actor)->SetItemType(NO_ITEM);
			}
		}
		App->animator->UnitPlay((Unit*)actor);
		timer.Start();
		return true;
	}

	//Functionality ---------
	bool Execute()
	{
		//Actor attack the target
		return timer.Read() > time;
	}

	//Get Methods -----------
	uint GetTime()const
	{
		return time;
	}

private:

	j1Timer timer;
	uint time = 0;

};
/// ---------------------------------------------

/// ---------------------------------------------
class HealUnitAction : public Action
{
public:
	HealUnitAction(Unit* actor, Unit* target) : Action(actor, TASK_U_HEAL_U), target(target)
	{};
	~HealUnitAction() {};

public:

	bool Execute()
	{
		if (target == nullptr) return true;

		return ((Unit*)actor)->HealUnit(target);
	}


private:
	Unit* target = nullptr;

};
/// ---------------------------------------------

/// Recollect (pick resources) Action ----------------------------
//Recollect resources
class RecollectVillagerAction : public Action
{
public:

	//Constructor -----------
	RecollectVillagerAction(Villager* actor, Resource* target) : Action(actor, TASK_U_RECOLLECT), target(target)
	{
	}

	//Destructor ------------
	~RecollectVillagerAction()
	{
		this->actor = nullptr;
		this->target = nullptr;
	}

	//Functionality ---------
	bool Activation()
	{
		//Check it the resource hasn't been yet depleted
		if (target == nullptr)return false;
		if (target->GetEntityType() != RESOURCE) return false;

		//Set actor animation
		((Villager*)actor)->Focus(target->GetPositionRounded(), true);
		((Villager*)actor)->CheckRecollectResource(target->GetResourceType());

		return true;
	}

	bool Execute()
	{
		if (target->GetLife() == 0)
		{
			if (((Villager*)actor)->GetCurrentResources() > 0)
			{
				//If resource is deplated execute a SaveResourceAction
				Building* save_point = App->entities_manager->GetNearestSavePoint(actor->GetPositionRounded(), ((Villager*)actor)->GetResourceCollectedType(), actor->GetDiplomacy());
				if(save_point != nullptr)
				{
					((Villager*)actor)->AddAction((Action*)App->action_manager->SaveResourcesAction((Villager*)actor, save_point), TASK_CHANNELS::PRIMARY);
				}
			}

			//Reset the animation
			((Unit*)actor)->SetAction(IDLE);
			App->animator->UnitPlay((Villager*)actor);
			return true;
		}

		//Actor recollect
		return ((Villager*)actor)->Recollect(target);
	}

	//Get Methods -----------
	Resource*	GetTarget()const
	{
		return target;
	}

private:

	Resource* target = nullptr;

};
/// ---------------------------------------------

/// Recollect (saving resources) Action ----------------------------
//Save the unit resources in a building
class SaveResourcesVillagerAction : public Action
{
public:

	//Constructor -----------
	SaveResourcesVillagerAction(Villager* actor, Building* target) : Action(actor, TASK_U_SAVE_RESOURCES), target(target)
	{

	}

	//Destructor ------------
	~SaveResourcesVillagerAction()
	{
		this->actor = nullptr;
		target = nullptr;
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

		((Villager*)actor)->CheckCarryResource();
		return true;
	}

	bool Execute()
	{
		if (target == nullptr)return true;
		//Actor save resources
		return ((Villager*)actor)->SaveResources(target);
	}

	//Get Methods -----------
	Building*	GetTarget()const
	{
		return target;
	}

private:

	Building* target = nullptr;

};
/// ---------------------------------------------


//PASIVE ACTIONS_______________________________________________________________________________________________________________________________________________________________________________

///Scann for units action------------------------
class ScannAction : public Action
{
public:

	//Constructor -------------
	ScannAction(Unit* actor) : Action(actor, TASK_U_SCANN)
	{

	}

	//Destructor ------------
	~ScannAction()
	{
		this->actor = nullptr;
		surrounding_units.clear();
		surrounding_buildings.clear();
	}

public:

	bool Execute()
	{
		if (actor->GetWorker()->GetCurrentActionType() == TASK_U_DIE) return true;

		surrounding_units.clear();
		surrounding_buildings.clear();

		App->entities_manager->units_quadtree.CollectCandidates(surrounding_units, actor->GetVision());
		App->entities_manager->OrganizeByNearest(surrounding_units, actor->GetVision());

		App->entities_manager->buildings_quadtree.CollectCandidates(surrounding_buildings, actor->GetVision());


		uint size = surrounding_units.size();

		for (uint i = 0; i < size; i++)
		{
			if (actor->GetDiplomacy() != surrounding_units[i]->GetDiplomacy() && surrounding_units[i]->GetAction() != (DIE || DISAPPEAR))
			{
				actor->AddPriorizedAction(App->action_manager->AttackToUnitAction((Unit*)actor, surrounding_units[i]));
				return false;
			}
		};

		uint sizeb = surrounding_buildings.size();

		for (uint k = 0; k < sizeb; k++)
		{
			if (actor->GetDiplomacy() != surrounding_buildings[k]->GetDiplomacy())
			{
				actor->AddPriorizedAction(App->action_manager->AttackToBuildingAction((Unit*)actor, surrounding_buildings[k]));
				return false;
			}
		};

		return false;
	};




private:

	std::vector<Unit*> surrounding_units;
	std::vector<Building*> surrounding_buildings;
};
///----------------------------------------------


///Autoattack------------------------------------
class AutoAttackPassiveAction : public Action
{
public:
	AutoAttackPassiveAction(Unit* actor) : Action(actor, TASK_U_AA)
	{};
	~AutoAttackPassiveAction() {};

public:

	bool Execute()
	{
		//Check if the unit is dying
		if (actor->GetWorker()->GetCurrentActionType() == TASK_U_DIE) return true;

		//Chek if it is a unit that can attack
		if (((Unit*)actor)->GetUnitType() != WIZARD_CHMP && ((Unit*)actor)->GetUnitType() != MONK) return true;

		surrounding_units.clear();
		surrounding_buildings.clear();

		//Units scann and attack
		uint size = surrounding_units.size();
		for (uint i = 0; i < size; i++)
		{
			if (actor->GetDiplomacy() != surrounding_units[i]->GetDiplomacy() && surrounding_units[i]->GetAction() != (DIE || DISAPPEAR))
			{
				actor->AddAction(App->action_manager->AttackToUnitAction((Unit*)actor, surrounding_units[i]), PRIMARY);
				return false;
			}
		};


		//Buildings scann and attack
		uint sizeb = surrounding_buildings.size();
		for (uint k = 0; k < sizeb; k++)
		{
			if (actor->GetDiplomacy() != surrounding_buildings[k]->GetDiplomacy())
			{
				actor->AddAction(App->action_manager->AttackToBuildingAction((Unit*)actor, surrounding_buildings[k]), PRIMARY);
				return false;
			}
		};

		return false;
	};


	void CleanUp()
	{
		this->actor = nullptr;
		surrounding_units.clear();
		surrounding_buildings.clear();
	};

private:
	std::vector<Unit*>		surrounding_units;
	std::vector<Building*>	surrounding_buildings;

};
///----------------------------------------------

///AutoHeal--------------------------------------
class AutoHealPassiveAction : public Action
{
public:
	AutoHealPassiveAction(Unit* actor) : Action(actor, TASK_U_AH)
	{};
	~AutoHealPassiveAction() {};

public:

	bool Execute()
	{
		surrounding_units.clear();

		//Units scann and attack
		uint size = surrounding_units.size();
		for (uint i = 0; i < size; i++)
		{
			if (actor->GetDiplomacy() == surrounding_units[i]->GetDiplomacy() && surrounding_units[i]->GetAction() != (DIE || DISAPPEAR))
			{
				actor->AddAction(new HealUnitAction((Unit*)actor, surrounding_units[i]));
				return false;
			}
		};
		return false;
	};

	void CleanUp()
	{
		this->actor = nullptr;
		surrounding_units.clear();
	};


private:

	std::vector<Unit*> surrounding_units;

};
///----------------------------------------------


#endif // !__ACTION_UNIT_H__


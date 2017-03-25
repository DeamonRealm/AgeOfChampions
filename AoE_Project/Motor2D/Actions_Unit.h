#ifndef __ACTION_UNIT_H__
#define __ACTION_UNIT_H__

#include "j1ActionManager.h"
#include "BaseEntities.h"
#include "j1App.h"
#include "j1EntitiesManager.h"

/// Move Unit Action ----------------------------
//Move the character
class MoveUnitAction : public Action
{
public:

	//Constructor -----------
	MoveUnitAction(Unit* actor, int x, int y) : Action(actor), x_new(x), y_new(y)
	{
		//Set path so the unit starts moving
		App->entities_manager->SetUnitPath((Unit*)actor, iPoint(x_new, y_new));
	}

	//Functionality ---------
	virtual bool execute()
	{
		return ((Unit*)actor)->Move();
	};

private:

	int x_new = 0;
	int y_new = 0;
	bool completed = false;

};
/// ---------------------------------------------

/// Attack Unit Action ----------------------------
//Move the character
class AttackUnitAction : public Action
{
public:

	//Constructor -----------
	AttackUnitAction(Unit* actor,Unit* target) : Action(target)
	{

	}

	//Functionality ---------
	virtual bool execute()
	{
		//Set actor interaction target
		((Unit*)actor)->SetInteractionTarget(target);
		//Actor attack the target
		((Unit*)actor)->Attack();
		
	};

private:

	Entity* target = nullptr;
	bool completed = false;

};
/// ---------------------------------------------
#endif // !__ACTION_UNIT_H__


#ifndef __ACTION_UNIT_H__
#define __ACTION_UNIT_H__

#include "Action.h"


//Move the character
class MoveUnitAction : public Action
{
public:
	MoveUnitAction(Unit* actor, int x, int y): Action(),
		 actor(actor), x_new(x), y_new(y)
	{};


	virtual void execute()
	{
		//Try changing so is SetPath from unit??? Maybe a ->SetPath overwrite??
		App->entities_manager->SetUnitPath(actor, iPoint(x_new, y_new));
		//-------
		actor->Move();
	};

private:
	Unit* actor;
	int x_new, y_new;
};



#endif // !__ACTION_UNIT_H__


#ifndef __ACTION_H__
#define __ACTION_H__


#include "j1EntitiesManager.h"

//Action virtual class (are commands but not for the console)
class Action
{
public:
	Action() {};
	~Action() {};
public:
	//This function defines the action taking place
	virtual void execute() {}
	virtual bool Isdone() { return true; };

	///Each diferent action inheriting from this class should have custom
	///properties to do its actions.
};









#endif // !__ACTION_H__


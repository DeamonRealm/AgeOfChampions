#ifndef __ACTION_H__
#define __ACTION_H__

#include "j1App.h"
#include "j1EntitiesManager.h"

//Action virtual class (are commands but not for the console)
class Action
{
public:
	//Set all the Actions to a list on the Entities manager to clean them up at closing the app.
	Action()
	{
		App->entities_manager->entity_actions.push_back(this);
	};
	~Action() {};
public:
	//This function defines the action taking place
	virtual void execute() {}
	virtual bool Isdone() { return true; };

	///Each diferent action inheriting from this class should have custom
	///properties to do its actions.
};



#endif // !__ACTION_H__


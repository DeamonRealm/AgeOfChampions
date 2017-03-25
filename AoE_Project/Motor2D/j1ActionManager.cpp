#include "j1ActionManager.h"

//Action class---------------------
Action::Action()
{
	App->entities_manager->entity_actions.push_back(this);
	
}
Action::~Action()
{
}
//---------------------------------



//Action Manager

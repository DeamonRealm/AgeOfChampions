#include "Villager.h"
#include "j1App.h"
#include "j1EntitiesManager.h"
#include "p2Log.h"


//Constructors ========================
Villager::Villager()
{

}

//Destructors =========================
Villager::~Villager()
{

}

//Game Loop ===========================
bool Villager::Update()
{
	if (path != nullptr)return Move();
	if (interaction_target != nullptr)return Recollect();
	return false;
}

//Functionality =======================
//Actions --------------
bool Villager::Interact()
{
	if (interaction_target == nullptr)
	{
		LOG("Error Interacting: Target NULL");
		return false;
	}

	//Calculate the distance between the unit and the resource 
	double distance = sqrt(abs(interaction_target->GetPositionRounded().DistanceNoSqrt(iPoint(position.x, position.y))));
	//Check if the resource is in the action area of the villager
	if (view_area < distance)
	{
		App->entities_manager->SetUnitPath(this, interaction_target->GetPositionRounded());
		return false;
	}

	//If the target is in the interaction area the unit do the correct action with it
	switch (interaction_target->GetEntityType())
	{
	case UNIT:		Attack();		break;
	case RESOURCE:	Recollect();	break;
	case BUILDING:	Cover();		break;
	}
	return true;
}

bool Villager::Recollect()
{
	//Extract the correct resources
	switch (((Resource*)interaction_target)->GetResourceType())
	{
	case TREE:
		break;
	case TREE_CUT:
		break;
	case CHOP:
		break;
	case BERRY_BUSH:
		break;
	case GOLD_ORE:
		break;
	case TINY_GOLD_ORE:
		break;
	case STONE_ORE:
		break;
	case TINY_STONE_ORE:
		break;
	}
	
	//CHeck if the resource is empty
	if (((Resource*)interaction_target)->GetCurrentResources() == 0)
	{
		//Delete resource of entities manager list
		return false;
	}
}

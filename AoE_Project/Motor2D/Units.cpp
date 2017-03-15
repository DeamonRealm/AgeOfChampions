#include "Units.h"
#include "j1App.h"
#include "j1EntitiesManager.h"
#include "p2Log.h"

/// Class Villager --------------------
//Constructors ========================
Villager::Villager()
{

}

Villager::Villager(const Villager & copy) :Unit(copy), item_type(copy.item_type),resources_capacity(copy.resources_capacity), current_resources(copy.current_resources), recollect_capacity(copy.recollect_capacity), recollect_rate(copy.recollect_rate)
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
//Get Methods ----------
ITEM_TYPE Villager::GetItemType() const
{
	return this->item_type;
}
//Set Methods ----------
void Villager::SetItemType(ITEM_TYPE type)
{
	item_type = type;
}
void Villager::SetResourcesCapacity(uint value)
{
	resources_capacity = value;
}
void Villager::SetRecollectCapacity(uint value)
{
	recollect_capacity = value;
}
void Villager::SetRecollectRate(uint value)
{
	recollect_rate = value;
}
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
		case UNIT:		if (action_timer.Read() > attack_rate)Attack();			break;
		case RESOURCE:	if (action_timer.Read() > recollect_rate)Recollect();	break;
		case BUILDING:	Cover();												break;
	}
	return true;
}

bool Villager::Recollect()
{
	//Get resources from the target resource
	uint recolect_value = MIN(recollect_capacity, resources_capacity - current_resources);
	
	//Extract resource material, if it fails return false
	if (!((Resource*)interaction_target)->ExtractResources(&recolect_value)) return false;
	
	//Add extracted resources at the villager
	current_resources += recolect_value;

	//If villager is full let's find a place to download
	if (current_resources == resources_capacity)
	{
		//Go to the nearest download point
	}

	//Reset interaction timer
	action_timer.Start();

	return true;
}

/// -----------------------------------

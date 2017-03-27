#include "Units.h"
#include "j1App.h"
#include "j1EntitiesManager.h"
#include "j1ActionManager.h"
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
bool Villager::Recollect()
{

	//Check if the target resource is in the "attack" (in this case used for recollect) area
	if (!attack_area.Intersects(&interaction_target->GetPositionRounded()))
	{
		iPoint goal = attack_area.NearestPoint(((Resource*)interaction_target)->GetInteractArea());
		this->AddPriorizedAction((Action*)App->action_manager->MoveAction(this, goal.x, goal.y));
		return false;
	}

	//Check the action rate
	if (action_timer.Read() < attack_rate) return false;

	//Check unit animation
	CheckRecollectAnimation(((Resource*)interaction_target)->GetResourceType());

	//Get resources from the target resource
	uint recollect_value = MIN(recollect_capacity, resources_capacity - current_resources);
	
	//Extract resource material, if it fails return true to end the recollect action
	if (!((Resource*)interaction_target)->ExtractResources(&recollect_value)) return true;
	
	//Add extracted resources at the villager
	current_resources += recollect_value;

	//If villager is full let's find a place to download
	if (current_resources == resources_capacity)
	{
		//Go to the nearest download point
		Building* save_point = App->entities_manager->SearchNearestSavePoint(GetPositionRounded());
		AddPriorizedAction((Action*)App->action_manager->SaveResourcesAction(this, save_point));
		return true;
	}

	//Reset interaction timer
	action_timer.Start();

	return false;
}

bool Villager::SaveResources()
{

	return true;
}

void Villager::CheckRecollectAnimation(RESOURCE_TYPE type)
{
	bool changed = false;
	switch (type)
	{
	case NO_RESOURCE:
		break;
	case TREE:
	case TREE_CUT:
	case CHOP:
		if (item_type != ITEM_TYPE::AXE)
		{
			item_type = AXE;
			action_type = ATTATCK;
			changed = true;
		}
		break;
	case BERRY_BUSH:
		if (item_type != ITEM_TYPE::BASKET)
		{
			item_type = BASKET;
			action_type = ATTATCK;
			changed = true;
		}
		break;
	case GOLD_ORE:
	case TINY_GOLD_ORE:
	case STONE_ORE:
	case TINY_STONE_ORE:
		if (item_type != ITEM_TYPE::PICK)
		{
			item_type = PICK;
			action_type = ATTATCK;
			changed = true;
		}
		break;
	}

	if (changed)App->animator->UnitPlay(this);
}

/// -----------------------------------

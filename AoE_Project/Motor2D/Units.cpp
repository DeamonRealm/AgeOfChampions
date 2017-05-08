#include "Units.h"
#include "j1App.h"
#include "j1EntitiesManager.h"
#include "j1ActionManager.h"
#include "p2Log.h"
#include "Hud_GamePanel.h"
#include "j1Player.h"
#include "j1AI.h"
#include "j1FogOfWar.h"
#include "j1Pathfinding.h"
#include "j1Map.h"

/// Class Villager --------------------
//Constructors ========================
Villager::Villager() : resource_collected_type(PLAYER_RESOURCES::GP_NO_RESOURCE)
{

}

Villager::Villager(const Villager & copy) : Unit(copy), item_type(copy.item_type), resource_collected_type(copy.resource_collected_type), resources_capacity(copy.resources_capacity), current_resources(copy.current_resources), recollect_capacity(copy.recollect_capacity), stone_recollect_rate(copy.stone_recollect_rate), gold_recollect_rate(copy.gold_recollect_rate), tree_recollect_rate(copy.tree_recollect_rate), bush_recollect_rate(copy.bush_recollect_rate)
{

}

//Destructors =========================
Villager::~Villager()
{

}


//Functionality =======================
//Save/Load methods ----
void Villager::SaveAsDef(pugi::xml_node & node)
{
	//Add all this unit attributes at the xml node

	//Name
	node.append_attribute("name") = "Villager";

	/*Type*/	node.append_attribute("unit_type") = App->animator->EnumToUnitStr(unit_type);
	/*Class*/	node.append_attribute("unit_class") = App->animator->EnumToUnitClassStr(unit_class);
	/*Atk Type*/node.append_attribute("attack_type") = App->animator->EnumToAttackStr(attack_type);

	/*Selec X*/ node.append_attribute("selection_x") = selection_rect.x;
	/*Selec Y*/ node.append_attribute("selection_y") = selection_rect.y;
	/*Selec W*/ node.append_attribute("selection_w") = selection_rect.w;
	/*Selec H*/ node.append_attribute("selection_h") = selection_rect.h;

	/*Icon X*/	node.append_attribute("icon_x") = icon_rect.x;
	/*Icon Y*/	node.append_attribute("icon_y") = icon_rect.y;
	/*Icon W*/	node.append_attribute("icon_w") = icon_rect.w;
	/*Icon H*/	node.append_attribute("icon_h") = icon_rect.h;

	/*Vision rad*/	node.append_attribute("vision_rad") = vision.GetRad();
	/*Mark Rad*/	node.append_attribute("mark_rad") = mark.GetRad();
	/*Soft Rad*/	node.append_attribute("soft_rad") = soft_collider.GetRad();
	/*Hard Rad*/	node.append_attribute("hard_rad") = hard_collider.GetRad();

	/*Max Life*/	node.append_attribute("max_life") = max_life;
	/*Life Pos X*/	node.append_attribute("life_pos_x") = life_position.x;
	/*Life Pos Y*/	node.append_attribute("life_pos_y") = life_position.y;
	/*Speed*/		node.append_attribute("speed") = speed;
	/*Atk delay*/	node.append_attribute("attack_delay") = attack_delay;
	/*Atk HitP*/	node.append_attribute("attack_hitpoints") = attack_hitpoints;
	/*Siege HitP*/	node.append_attribute("siege_hitpoints") = siege_hitpoints;
	/*Atk Rate*/	node.append_attribute("attack_rate") = attack_rate;
	/*Atk Range*/	node.append_attribute("attack_range") = attack_area.GetRad();
	/*Defense*/		node.append_attribute("defense") = defense;
	/*Food Cost*/	node.append_attribute("food_cost") = food_cost;
	/*Wood Cost*/	node.append_attribute("wood_cost") = wood_cost;
	/*Gold Cost*/	node.append_attribute("gold_cost") = gold_cost;
	/*Popu Cost*/	node.append_attribute("population_cost") = population_cost;
	/*Train Time*/	node.append_attribute("train_time") = train_time;
	/*Experience*/	node.append_attribute("unit_experience") = unit_experience;

	/*Resource Capacity*/	node.append_attribute("resources_capacity") = resources_capacity;
	/*Recollect Capacity*/	node.append_attribute("recollect_capacity") = recollect_capacity;
	/*Stone Rec Rate*/		node.append_attribute("stone_recollect_rate") = stone_recollect_rate;
	/*Gold Rec Rate*/		node.append_attribute("gold_recollect_rate") = gold_recollect_rate;
	/*Tree Rec Rate*/		node.append_attribute("tree_recollect_rate") = tree_recollect_rate;
	/*Bush Rec Rate*/		node.append_attribute("bush_recollect_rate") = bush_recollect_rate;
}

//Actions -----
bool Villager::Die()
{

	if (action_type != DIE && action_type != DISAPPEAR)
	{
		if (GetDiplomacy() == DIPLOMACY::ALLY)
		{
			App->player->game_panel->IncreaseDeathAllies();
			App->entities_manager->GetExperienceFromUnit(unit_experience, DIPLOMACY::ALLY);
		}
		if (GetDiplomacy() == DIPLOMACY::ENEMY)
		{
			//App->player->game_panel->IncreaseDeathEnemies();
			App->entities_manager->GetExperienceFromUnit(unit_experience, DIPLOMACY::ENEMY);

			std::list<Unit*>::const_iterator it = App->entities_manager->UnitsList()->begin();
			bool lastenemy = true;
			while (it != App->entities_manager->UnitsList()->end())
			{
				if (it._Ptr->_Myval->GetDiplomacy() == ENEMY && it._Ptr->_Myval->GetEntityType() == UNIT && it._Ptr->_Myval != this)
					lastenemy = false;
				it++;
			}
			if (lastenemy && !App->debug_mode) App->player->game_panel->DoWin();
		}



		App->buff_manager->RemoveTargetBuffs(this);
		action_type = DIE;
		if (this->GetDiplomacy() == ALLY) App->player->game_panel->IncressPopulation(-1, false);
		App->entities_manager->AddDeathUnit(this);
		if (item_type == GOLD || item_type == STONE || item_type == MEAT || item_type == WOOD)
		{
			item_type = NO_ITEM;
		}
		App->animator->UnitPlay(this);
		App->fog_of_war->ReleaseEntityFog(this);
	}
	else if (current_animation->IsEnd())
	{
		if (action_type == DIE)
		{
			action_type = DISAPPEAR;
			App->animator->UnitPlay(this);
		}
		else
		{
			if (GetDiplomacy() == DIPLOMACY::ENEMY)
			{
				App->player->game_panel->IncreaseDeathEnemies();
			}
			App->entities_manager->DeleteEntity(this);
			return true;
		}
	}
	return false;
}

//Get Methods ----------
ITEM_TYPE Villager::GetItemType() const
{
	return this->item_type;
}

PLAYER_RESOURCES Villager::GetResourceCollectedType() const
{
	return resource_collected_type;
}

uint Villager::GetCurrentResources() const
{
	return current_resources;
}

uint Villager::GetRecollectCapacity() const
{
	return recollect_capacity;
}

uint Villager::GetResourcesCapacity() const
{
	return resources_capacity;
}

uint Villager::GetRecollectRate(RESOURCE_TYPE type) const
{
	switch (type)
	{
	case TREE:
	case TREE_CUT:
	case CHOP:
		return tree_recollect_rate;
		break;
	case BERRY_BUSH:
		return bush_recollect_rate;
		break;
	case GOLD_ORE:
	case TINY_GOLD_ORE:
		return gold_recollect_rate;
		break;
	case STONE_ORE:
	case TINY_STONE_ORE:
		return stone_recollect_rate;
		break;
	};
}

//Set Methods ----------
void Villager::SetItemType(ITEM_TYPE type)
{
	item_type = type;
}

void Villager::SetResourceCollectedType(PLAYER_RESOURCES type)
{
	resource_collected_type = type;
}

void Villager::SetResourcesCapacity(uint value)
{
	resources_capacity = value;
}
void Villager::SetRecollectCapacity(uint value)
{
	recollect_capacity = value;
}
void Villager::SetRecollectRate(uint value, RESOURCE_TYPE res_type)
{
	switch (res_type)
	{
	case TREE:
		tree_recollect_rate = value;
		break;
	case BERRY_BUSH:
		bush_recollect_rate = value;
		break;
	case GOLD_ORE:
		gold_recollect_rate = value;
		break;
	case STONE_ORE:
		stone_recollect_rate = value;
		break;
	}
}

void Villager::SetCurrentResources(uint value)
{
	current_resources = value;
}
//Actions --------------
bool Villager::Recollect(Resource* target)
{
	if (target == nullptr)return true;

	//Check if the target resource is in the "attack" (in this case used for recollect) area
	if (!attack_area.Overlap(target->GetInteractArea()))
	{
		iPoint goal = attack_area.NearestPoint(target->GetInteractArea());
		iPoint woop = App->map->WorldToMap(goal.x,goal.y);
		iPoint woop_tile = App->map->WorldToMap(target->GetPositionRounded().x, target->GetPositionRounded().y);

		if (!App->pathfinding->IsWalkable(woop)) {
			goal = this->FindWalkableAdjacent(target->GetPositionRounded());
			if (goal == iPoint(-1, -1))
				return true;
			LOG("GOAL %i %i ", goal.x, goal.y);
			LOG("GOAL %i %i ", woop.x, woop.y);
			LOG("GOAL %i %i ", woop_tile.x, woop_tile.y);
		}


		App->pathfinding->PushPath(this, goal, target->GetPositionRounded());

		//this->AddPriorizedAction((Action*)App->action_manager->MoveAction(this, goal, (*target)->GetPositionRounded()));
		return false;
	}

	//Check the action rate
	uint rate_value = GetRecollectRate(target->GetResourceType());
	if (action_timer.Read() < rate_value) return false;

	//Get resources from the target resource
	uint recollect_value = MIN(recollect_capacity, resources_capacity - current_resources);

	//Extract resource material, if it fails return true to end the recollect action
	if (!target->ExtractResources(&recollect_value))
	{
		target = nullptr;
		if (current_resources > 0)
		{
			//Go to the nearest download point
			Building* save_point = App->entities_manager->GetNearestSavePoint(GetPositionRounded(),this->resource_collected_type, this->GetDiplomacy());
			if (save_point == nullptr)return true;
			//Set the carry action animation type
			AddAction((Action*)App->action_manager->SaveResourcesAction(this, save_point), TASK_CHANNELS::PRIMARY);
			return true;
		}
		else
		{
			this->action_type = IDLE;
			App->animator->UnitPlay(this);
		}
		return true;
	}

	//Add extracted resources at the villager
	current_resources += recollect_value;

	//If villager is full let's find a place to download
	if (current_resources == resources_capacity)
	{
		//Go to the nearest download point
		Building* save_point = App->entities_manager->GetNearestSavePoint(GetPositionRounded(), this->resource_collected_type, this->GetDiplomacy());
		if (save_point == nullptr)return true;
		//Set the carry action animation type
		AddPriorizedAction((Action*)App->action_manager->SaveResourcesAction(this, save_point));
		return false;
	}

	//Reset interaction timer
	action_timer.Start();

	return false;
}

bool Villager::SaveResources(Building* save_point)
{
	//Check if the target building is in the "attack" (in this case used for save resources) area
	if (!attack_area.Intersects(save_point->GetInteractArea()))
	{
		iPoint intersect_point = attack_area.NearestPoint(save_point->GetInteractArea());
		App->pathfinding->PushPath(this, intersect_point);

		//this->AddPriorizedAction((Action*)App->action_manager->MoveAction(this, iPoint(intersect_point.x, intersect_point.y)));
		return false;
	}

	//Store all the resources collected in the player or AI bag
	if (save_point->GetDiplomacy() == ALLY)
		App->player->game_panel->AddResource(current_resources, resource_collected_type);
	if (save_point->GetDiplomacy() == ENEMY)
		App->AI->AddResources(resource_collected_type, current_resources);

	//Reset all the resources data so the next action will not be affected for it
	this->ResetResourcesData();
	App->animator->UnitPlay(this);

	return true;
}

void Villager::CheckRecollectResource(RESOURCE_TYPE type)
{
	bool changed = false;
	switch (type)
	{
	case TREE:
	case TREE_CUT:
	case CHOP:
		if (resource_collected_type != GP_WOOD)
		{
			ResetResourcesData();
			resource_collected_type = GP_WOOD;
		}
		if (item_type != ITEM_TYPE::AXE)
		{
			item_type = AXE;
			changed = true;
		}
		break;
	case BERRY_BUSH:
		if (resource_collected_type != GP_MEAT)
		{
			ResetResourcesData();
			resource_collected_type = GP_MEAT;
		}
		if (item_type != ITEM_TYPE::BASKET)
		{
			item_type = BASKET;
			changed = true;
		}
		break;
	case GOLD_ORE:
	case TINY_GOLD_ORE:
		if (resource_collected_type != GP_GOLD)
		{
			ResetResourcesData();
			resource_collected_type = GP_GOLD;
		}
		if (item_type != ITEM_TYPE::PICK)
		{
			item_type = PICK;
			changed = true;
		}
		break;
	case STONE_ORE:
	case TINY_STONE_ORE:
		if (resource_collected_type != GP_STONE)
		{
			ResetResourcesData();
			resource_collected_type = GP_STONE;
		}
		if (item_type != ITEM_TYPE::PICK)
		{
			item_type = PICK;
			changed = true;
		}
		break;
	}

	action_type = ATTATCK;
	App->animator->UnitPlay(this);
}

void Villager::CheckCarryResource()
{
	switch (resource_collected_type)
	{
	case GP_NO_RESOURCE:
		item_type = NO_ITEM;
		break;
	case GP_WOOD:
		item_type = ITEM_TYPE::WOOD;
		break;
	case GP_MEAT:
		item_type = ITEM_TYPE::MEAT;
		break;
	case GP_GOLD:
		item_type = ITEM_TYPE::GOLD;
		break;
	case GP_STONE:
		item_type = ITEM_TYPE::STONE;
		break;
	}
}

void Villager::ResetResourcesData()
{
	item_type = NO_ITEM;
	resource_collected_type = GP_NO_RESOURCE;
	current_resources = 0;
	action_type = IDLE;
}

/// -----------------------------------

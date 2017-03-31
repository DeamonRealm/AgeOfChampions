#include "j1GroupMovement.h"
#include "j1App.h"
#include "j1Pathfinding.h"
#include "j1Map.h"
#include "j1ActionManager.h"
j1GroupMovement::j1GroupMovement()
{
}

j1GroupMovement::~j1GroupMovement()
{
}

iPoint j1GroupMovement::GetMiddlePoint()
{
	iPoint ret = { 0,0 };
	for (std::list<Entity*>::iterator item = units->begin(); item != units->end(); item++)
	{
		Entity* single_unit = item._Ptr->_Myval;
		ret += single_unit->GetPositionRounded();
	}
	ret /= units->size();
	//Get all positions and add them on one ipoint and then divide them with the size of the units 
	// if that position is walkable return that position if is not look destination and get values 
	iPoint checker = App->map->WorldToMap(ret.x, ret.y);

	while (!App->pathfinding->IsWalkable(checker)) {
		if (checker.x < destination.x)	checker.x++;
		else checker.x--;
		if (checker.y < destination.y)	checker.y++;
		else checker.y--;
	}
	ret = App->map->MapToWorldCenter(checker.x, checker.y);
	return ret;
}

void j1GroupMovement::GetGroupOfUnits(std::list<Entity*>* get ,int x, int y)
{
	map_destination = App->map->WorldToMap(x,y);
	destination = { x, y };

	if (!App->pathfinding->IsWalkable(map_destination)) 
	{
		return;
	}
		//Get the list of units
		units = get;
		//get the size of units
		group_size = units->size();
		//Get the middle point and set the lead of formation
		middle_point = GetMiddlePoint();
		lead = *(units->begin());
		//Create the lead path
		std::vector<iPoint>* first_destination = nullptr;
		std::vector<iPoint>* second_destination = nullptr;
	//	first_destination = CreateFirstDestination();
		second_destination = LeaderPath();
		//second_destination->insert(second_destination->end(), first_destination->begin(), first_destination->end());
		//push action
		lead->AddAction((Action*)App->action_manager->MoveAction(second_destination, (Unit*)lead));
		lead_direcction = ((Unit*)lead)->LookDirection(*(second_destination->begin()), *(second_destination->begin() + 1));
		//do the other units path
		OtherUnitsPath();
	
}

void j1GroupMovement::OtherUnitsPath()
{
	iPoint new_middle_point;
	iPoint new_destination;
	int i = 1;

	switch (lead_direcction)
	{
	case NORTH:
		//iterate list of units
		for (std::list<Entity*>::iterator item = units->begin(); item != units->end(); item++)
		{
			//get the unit if is the lead continue
			Entity* single_unit = item._Ptr->_Myval;
			if (single_unit == lead) {
				continue;
			}
			//set path for the other units
			std::vector<iPoint>* unit_destination = nullptr;
			new_destination.create(destination.x + 20 * i, destination.y - 20 * i);
			unit_destination = App->pathfinding->SimpleAstar(single_unit->GetPositionRounded(), new_destination);
			single_unit->AddAction((Action*)App->action_manager->MoveAction(unit_destination, (Unit*)single_unit));
			i++;

		}
		break;
	case NORTH_EAST:
		//iterate list of units
		for (std::list<Entity*>::iterator item = units->begin(); item != units->end(); item++)
		{
			//get the unit if is the lead continue
			Entity* single_unit = item._Ptr->_Myval;
			if (single_unit == lead) {
				continue;
			}
			//set path for the other units
			std::vector<iPoint>* unit_destination = nullptr;
			new_destination.create(destination.x + 20 * i, destination.y + 20 * i);
			unit_destination = App->pathfinding->SimpleAstar(single_unit->GetPositionRounded(), new_destination);
			single_unit->AddAction((Action*)App->action_manager->MoveAction(unit_destination, (Unit*)single_unit));
			i++;

		}
		break;
	case EAST:
		//iterate list of units
		for (std::list<Entity*>::iterator item = units->begin(); item != units->end(); item++)
		{
			//get the unit if is the lead continue
			Entity* single_unit = item._Ptr->_Myval;
			if (single_unit == lead) {
				continue;
			}
			//set path for the other units
			std::vector<iPoint>* unit_destination = nullptr;
			new_destination.create(destination.x, destination.y + 20 * i);
			unit_destination = App->pathfinding->SimpleAstar(single_unit->GetPositionRounded(), new_destination);
			single_unit->AddAction((Action*)App->action_manager->MoveAction(unit_destination, (Unit*)single_unit));
			i++;

		}
		break;
	case SOUTH_EAST:
		//iterate list of units
		for (std::list<Entity*>::iterator item = units->begin(); item != units->end(); item++)
		{
			//get the unit if is the lead continue
			Entity* single_unit = item._Ptr->_Myval;
			if (single_unit == lead) {
				continue;
			}
			//set path for the other units
			std::vector<iPoint>* unit_destination = nullptr;
			new_destination.create(destination.x - 20 * i, destination.y + 20 * i);
			unit_destination = App->pathfinding->SimpleAstar(single_unit->GetPositionRounded(), new_destination);
			single_unit->AddAction((Action*)App->action_manager->MoveAction(unit_destination, (Unit*)single_unit));
			i++;

		}
		break;
	case SOUTH:
		for (std::list<Entity*>::iterator item = units->begin(); item != units->end(); item++)
		{
			//get the unit if is the lead continue
			Entity* single_unit = item._Ptr->_Myval;
			if (single_unit == lead) {
				continue;
			}
			//set path for the other units
			std::vector<iPoint>* unit_destination = nullptr;
			new_destination.create(destination.x - 20 * i, destination.y + 20 * i);
			unit_destination = App->pathfinding->SimpleAstar(single_unit->GetPositionRounded(), new_destination);
			single_unit->AddAction((Action*)App->action_manager->MoveAction(unit_destination, (Unit*)single_unit));
			i++;

		}
		break;
	case SOUTH_WEST:
		for (std::list<Entity*>::iterator item = units->begin(); item != units->end(); item++)
		{
			//get the unit if is the lead continue
			Entity* single_unit = item._Ptr->_Myval;
			if (single_unit == lead) {
				continue;
			}
			//set path for the other units
			std::vector<iPoint>* unit_destination = nullptr;
			new_destination.create(destination.x + 20 * i, destination.y + 20 * i);
			unit_destination = App->pathfinding->SimpleAstar(single_unit->GetPositionRounded(), new_destination);
			single_unit->AddAction((Action*)App->action_manager->MoveAction(unit_destination, (Unit*)single_unit));
			i++;

		}
		break;
	case WEST:
		for (std::list<Entity*>::iterator item = units->begin(); item != units->end(); item++)
		{
			//get the unit if is the lead continue
			Entity* single_unit = item._Ptr->_Myval;
			if (single_unit == lead) {
				continue;
			}
			//set path for the other units
			std::vector<iPoint>* unit_destination = nullptr;
			new_destination.create(destination.x, destination.y - 20 * i);
			unit_destination = App->pathfinding->SimpleAstar(single_unit->GetPositionRounded(), new_destination);
			single_unit->AddAction((Action*)App->action_manager->MoveAction(unit_destination, (Unit*)single_unit));
			i++;

		}
		break;
	case NORTH_WEST:
		for (std::list<Entity*>::iterator item = units->begin(); item != units->end(); item++)
		{
			//get the unit if is the lead continue
			Entity* single_unit = item._Ptr->_Myval;
			if (single_unit == lead) {
				continue;
			}
			//set path for the other units
			std::vector<iPoint>* unit_destination = nullptr;
			new_destination.create(destination.x + 20 * i, destination.y - 20 * i);
			unit_destination = App->pathfinding->SimpleAstar(single_unit->GetPositionRounded(), new_destination);
			single_unit->AddAction((Action*)App->action_manager->MoveAction(unit_destination, (Unit*)single_unit));
			i++;

		}
		break;
	default:
		break;
	}
	/*
	//iterate list of units
	for (std::list<Entity*>::iterator item = units->begin(); item != units->end(); item++)
	{
		//get the unit if is the lead continue
		Entity* single_unit = item._Ptr->_Myval;
		if (single_unit == lead) {
			continue;
		}
		//set path for the other units
			std::vector<iPoint>* first_destination = nullptr;
			std::vector<iPoint>* second_destination = nullptr;
			//get closer new middle point STILL NEED IMPROVEMENT
		//	new_middle_point.create(middle_point.x + 30 * i, middle_point.y + 30 * i);
		//	first_destination = App->pathfinding->SimpleAstar(single_unit->GetPositionRounded(), new_middle_point);
			//get closer new destination STILL NEED IMPROVEMENT
			new_destination.create(destination.x + 20 * i, destination.y + 20 * i);
			second_destination = App->pathfinding->SimpleAstar(single_unit->GetPositionRounded(), new_destination);
			//second_destination->insert(second_destination->end(), first_destination->begin(), first_destination->end());
			//push action
			single_unit->AddAction((Action*)App->action_manager->MoveAction(second_destination, (Unit*)single_unit));
			i++;
		
	}
	*/
}

bool j1GroupMovement::GroupCanWalk(const iPoint & position)
{
	iPoint check = position;
	for (int i = 0; i < group_size; i++)
	{

	}
	return false;
}

std::vector<iPoint>* j1GroupMovement::CreatePath()
{
	std::vector<iPoint>* first_destination = nullptr;
	std::vector<iPoint>* second_destination = nullptr;

	first_destination = CreateFirstDestination();
	second_destination = LeaderPath();
	second_destination->insert(second_destination->end(), first_destination->begin(), first_destination->end());
	return second_destination;
}

std::vector<iPoint>* j1GroupMovement::CreateFirstDestination()
{
	std::vector<iPoint>* path = nullptr;
	path = App->pathfinding->SimpleAstar(lead->GetPositionRounded(),middle_point);

	//first leader do the path to the middle point
	//then other units do the path
	//if leader last position look north and south add on middle positive x on width and positive y on height
	//if leader last position look north-east and south-west add on middle positive x and y on width and negative x positive y on height
	//if leader last position look east and west add on middle 0 on x and y on width and negative y on height
	//if leader last position look south-east north-west add on middle negative on x and positive y on width and negative x negative y on height
	return path;
}

std::vector<iPoint>* j1GroupMovement::LeaderPath()
{
	std::vector<iPoint>* path = nullptr;
	path = App->pathfinding->SimpleAstar(lead->GetPositionRounded(), destination);

	//first leader do the path to the middle point
	//then other units COPY the path with changed values
	//if leader last position look north and south add on middle positive x on width and positive y on height
	//if leader last position look north-east and south-west add on middle positive x and y on width and negative x positive y on height
	//if leader last position look east and west add on middle 0 on x and y on width and negative y on height
	//if leader last position look south-east north-west add on middle negative on x and positive y on width and negative x negative y on height
	return path;

}

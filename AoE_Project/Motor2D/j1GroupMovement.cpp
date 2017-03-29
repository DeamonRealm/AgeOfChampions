#include "j1GroupMovement.h"
#include "j1App.h"
#include "j1Pathfinding.h"
GroupMovement::GroupMovement()
{
}

GroupMovement::~GroupMovement()
{
}

iPoint GroupMovement::GetMiddlePoint()
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
	while (!App->pathfinding->IsWalkable(ret)) {
		if (ret.x < destination.x)	ret.x++;
		else ret.x--;
		if (ret.y < destination.y)	ret.y++;
		else ret.y--;
	}
	return ret;
}

bool GroupMovement::GroupCanWalk(const iPoint & position)
{
	iPoint check = position;
	for (int i = 0; i < group_size; i++)
	{

	}
	return false;
}

std::vector<iPoint>* GroupMovement::CreatePath()
{
	std::vector<iPoint>* first_destination = nullptr;
	std::vector<iPoint>* second_destination = nullptr;

	first_destination = CreateFirstDestination();
	second_destination = LeaderPath();
	second_destination->insert(second_destination->end(), first_destination->begin(), first_destination->end());
	return second_destination;
}

std::vector<iPoint>* GroupMovement::CreateFirstDestination()
{
	middle_point = GetMiddlePoint();
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

std::vector<iPoint>* GroupMovement::LeaderPath()
{
	std::vector<iPoint>* path = nullptr;

	//first leader do the path to the middle point
	//then other units COPY the path with changed values
	//if leader last position look north and south add on middle positive x on width and positive y on height
	//if leader last position look north-east and south-west add on middle positive x and y on width and negative x positive y on height
	//if leader last position look east and west add on middle 0 on x and y on width and negative y on height
	//if leader last position look south-east north-west add on middle negative on x and positive y on width and negative x negative y on height
	return path;

}

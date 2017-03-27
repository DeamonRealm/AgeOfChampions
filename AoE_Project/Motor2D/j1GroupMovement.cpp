#include "j1GroupMovement.h"

GroupMovement::GroupMovement()
{
}

GroupMovement::~GroupMovement()
{
}

iPoint GroupMovement::GetMiddlePoint()
{
	//Get all positions and add them on one ipoint and then divide them with the size of the units 
	// if that position is walkable return that position if is not look destination and get values 
	return iPoint();
}

void GroupMovement::CreateFirstDestination()
{
	
	//first leader do the path to the middle point
	//then other units do the path
	//if leader last position look north and south add on middle positive x on width and positive y on height
	//if leader last position look north-east and south-west add on middle positive x and y on width and negative x positive y on height
	//if leader last position look east and west add on middle 0 on x and y on width and negative y on height
	//if leader last position look south-east north-west add on middle negative on x and positive y on width and negative x negative y on height

}

void GroupMovement::LeaderPath()
{
	//first leader do the path to the middle point
	//then other units COPY the path with changed values
	//if leader last position look north and south add on middle positive x on width and positive y on height
	//if leader last position look north-east and south-west add on middle positive x and y on width and negative x positive y on height
	//if leader last position look east and west add on middle 0 on x and y on width and negative y on height
	//if leader last position look south-east north-west add on middle negative on x and positive y on width and negative x negative y on height
}

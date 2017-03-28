#ifndef _J1_GROUP_MOVEMENT_
#define _J1_GROUP_MOVEMENT_

#include "j1Module.h"
#include "p2Point.h"
#include "j1EntitiesManager.h"

#define FORMATION_WIDTH_LIMIT 5
#define FORMATION_HEIGHT_LIMIT 5
enum FormationType {
	NO_FORMATION,
	SQUARE_CLOSE_FORMATION,
	SQUARE_SEPARATE_FORMATION
};
enum LeadOrientation {
	NO_LEAD_ORIENTATION,
	LEAD_NORTH,
	LEAD_NORTH_EAST,
	LEAD_EAST,
	LEAD_SOUTH_EAST,
	LEAD_SOUTH,
	LEAD_SOUTH_WEST,
	LEAD_WEST,
	LEAD_NORTH_WEST
};
struct Formation
{
	int formationSize = 0;
	int height = 0;
	int width = 0;
	FormationType formation_type = NO_FORMATION;

};
class GroupMovement
{
public:
	GroupMovement();
	~GroupMovement();

	//Functions
	//CreatePath
		//Set middle_point
		//leader path on middle_point other units path around the middle_point
		//leader do path to destination other units copy the path with minor modifications
		//return the path to units
	iPoint destination;
	iPoint GetMiddlePoint();
	bool GroupCanWalk(const iPoint& position);
	std::vector<iPoint>* CreatePath();
	std::vector<iPoint>* CreateFirstDestination();
	std::vector<iPoint>* LeaderPath();

private:
	// list of units selected
	// Pointer first unit "leader"
	std::list<Entity*>* units =nullptr;
	Entity* lead = nullptr;
	iPoint middle_point;
	Formation* formation = nullptr;
	int group_size = 0;
};

#endif // !_J1_GROUP_MOVEMENT_

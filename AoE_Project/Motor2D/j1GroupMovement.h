#ifndef _J1_GROUP_MOVEMENT_
#define _J1_GROUP_MOVEMENT_

#include "j1Module.h"
#include "p2Point.h"
#define FORMATION_WIDTH_LIMIT 5
#define FORMATION_HEIGHT_LIMIT 5
enum FormationType {
	NO_FORMATION,
	SQUARE_FORMATION
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
	iPoint middle_point;
	Formation* formation = nullptr;
	int group_size = 0;
};

#endif // !_J1_GROUP_MOVEMENT_

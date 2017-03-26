#ifndef _J1_GROUP_MOVEMENT_
#define _J1_GROUP_MOVEMENT_

#include "j1Module.h"
#include "p2Point.h"
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
private:
	// list of units selected
	// Pointer first unit "leader"
	iPoint middle_point;
	Formation* formation = nullptr;
	
};

#endif // !_J1_GROUP_MOVEMENT_

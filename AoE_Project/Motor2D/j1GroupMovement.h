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

struct Formation
{
	void SetFormation(int size);
	int formationSize = 0;
	int height = 0;
	int width = 0;
	FormationType formation_type = NO_FORMATION;

};
class j1GroupMovement:public j1Module
{
public:
	j1GroupMovement();
	~j1GroupMovement();

	//Functions
	//CreatePath
		//Set middle_point
		//leader path on middle_point other units path around the middle_point
		//leader do path to destination other units copy the path with minor modifications
		//return the path to units
	iPoint destination;
	iPoint map_destination;

	void GetGroupOfUnits(std::list<Entity*>* units, int x, int y, bool active);

private:
	iPoint GetMiddlePoint();
	void OtherUnitsPath(bool active);
	bool GroupCanWalk(const iPoint& position);
	bool PlaceTaken(const std::vector<iPoint> positions, const iPoint& check);
	std::vector<iPoint>* CreatePath();
	std::vector<iPoint>* CreateFirstDestination();
	std::vector<iPoint>* LeaderPath();
private:
	// list of units selected
	// Pointer first unit "leader"
	std::list<Entity*>* units =nullptr;
	Entity* lead = nullptr;
	iPoint middle_point;
	Formation formation;
	int group_size = 0;
	DIRECTION_TYPE lead_direcction;
};

#endif // !_J1_GROUP_MOVEMENT_

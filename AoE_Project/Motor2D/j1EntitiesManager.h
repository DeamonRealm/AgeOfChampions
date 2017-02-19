#ifndef _ENTITIES_MANAGER_
#define _ENTITIES_MANAGER_

#include "j1Module.h"
class Unit;
class Building;
class Resource;

class j1EntitiesManager : public j1Module
{
public:

	j1EntitiesManager();
	~j1EntitiesManager();

public:

	// Called before render is available
	bool Awake(pugi::xml_node& config_node);

	// Called before the first frame
	bool Start();

	// Called before quitting
	bool CleanUp();

private:

	//Lists of current game entities
	std::list<Unit*>		units;
	std::list<Resource*>	resources;
	std::list<Building*>	buildings;

	//Vector of pre-defined units
	std::vector<Unit*>		units_defs;
	std::vector<Resource*>	resoureces_defs;
	std::vector<Building*>	buildings_defs;

};
#endif // _ENTITIES_MANAGER_

#ifndef _ENTITIES_MANAGER_
#define _ENTITIES_MANAGER_

#include "j1Module.h"
#include "BaseEntities.h"

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

	// Called each loop iteration
	bool Update(float dt);

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

	//Boolean that defines the debug mode state
	bool debug = false;

public:

	//Functionality
	Unit*		GenerateUnit(UNIT_TYPE type);
	Building*	GenerateBuilding(BUILDING_TYPE type);

};
#endif // _ENTITIES_MANAGER_

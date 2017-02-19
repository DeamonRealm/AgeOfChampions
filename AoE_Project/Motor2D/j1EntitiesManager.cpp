#include "j1EntitiesManager.h"

#include "BaseEntities.h"

//Constructors ========================
j1EntitiesManager::j1EntitiesManager()
{

}

//Destructors =========================
j1EntitiesManager::~j1EntitiesManager()
{
}

//Game Loop ===========================
bool j1EntitiesManager::Awake(pugi::xml_node & config_node)
{
	return true;
}

bool j1EntitiesManager::Start()
{
	return true;
}

bool j1EntitiesManager::CleanUp()
{
	//Clean Up Units List
	std::list<Unit*>::iterator units_item = units.begin();
	while (units_item != units.end())
	{
		RELEASE(units_item._Ptr->_Myval);
		units_item++;
	}
	units.clear();

	//Clean Up resources list
	std::list<Resource*>::iterator resources_item = resources.begin();
	while (resources_item != resources.end())
	{
		RELEASE(resources_item._Ptr->_Myval);
		resources_item++;
	}
	resources.clear();

	//Clean Up buildings list
	std::list<Building*>::iterator buildings_item = buildings.begin();
	while (buildings_item != buildings.end())
	{
		RELEASE(buildings_item._Ptr->_Myval);
		buildings_item++;
	}
	buildings.clear();

	//Clean Up units_defs vector
	uint size = units_defs.size();
	if (size > 0)
	{
		size--;
		for (uint k = size; k >= 0; k--)
		{
			RELEASE(units_defs[k]);
		}
		units_defs.clear();
	}

	//Clean Up resoureces_defs vector
	size = resoureces_defs.size();
	if (size > 0)
	{
		size--;
		for (uint k = size; k >= 0; k--)
		{
			RELEASE(resoureces_defs[k]);
		}
		resoureces_defs.clear();
	}
	//Clean Up buildings_defs vector
	size = buildings_defs.size();
	if (size > 0)
	{
		size--;
		for (uint k = size; k >= 0; k--)
		{
			RELEASE(buildings_defs[k]);
		}
		buildings_defs.clear();
	}

	return true;
}

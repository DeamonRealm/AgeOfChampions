#ifndef _BUILDINGS_
#define _BUILDINGS_

#include "BaseEntities.h"

/// Habitable Building --------------------------
//Class that defines the base for habitable buildings
class HabitableBuilding : public Building
{
public:

	HabitableBuilding();
	HabitableBuilding(const HabitableBuilding& copy);
	~HabitableBuilding();

private:

	uint				units_capacity = 0;
	uint				current_units = 0;
	iPoint				units_spawn_point = { 0,0 };
	std::list<Unit*>	units_in;

public:

	//Functionality -------------------
	//Get units in list
	std::list<Unit*>	GetUnitsIn()const;

	//Cover / Release units -
	bool	CoverUnit(const Unit* target);
	void	ReleaseUnit(const Unit* target);
	void	ReleaseAllUnits();

	//Set Methods -----------
	void	SetUnitsCapacity(uint capacity);
	void	SetCurrentUnits(uint units);
	void	SetUnitsSpawnPoint(const iPoint& point);

	//Get Methods -----------
	uint	GetUnitsCapacity()const;
	uint	GetCurrentUnits()const;
	iPoint	GetSpawnPoint()const;

};
/// ---------------------------------------------

/// Productive Building ------------------------
//Base class that define the productive buildings attributes
class ProductiveBuilding :public HabitableBuilding
{
public:

	ProductiveBuilding();
	ProductiveBuilding(const ProductiveBuilding& copy);
	~ProductiveBuilding();

private:

	std::priority_queue<Unit*>	production_queue;
	uint						production_capacity = 10;

public:

	//Functionality ----------------------

	//Set Methods -----------
	void SetProductionCapacity(uint capacity);

	//Get Methods -----------
	uint GetProductionCapacity()const;
	std::priority_queue<Unit*> GetProductionQueue()const;

};
/// ---------------------------------------------
#endif // _BUILDINGS_
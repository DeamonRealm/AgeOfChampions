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

/// Town Center Building ------------------------
class TownCenter :public HabitableBuilding
{
public:

	TownCenter();
	TownCenter(const TownCenter& copy);
	~TownCenter();

private:

	std::priority_queue<Unit*, std::vector<Unit*>>	production_queue;

};
/// ---------------------------------------------
#endif // _BUILDINGS_
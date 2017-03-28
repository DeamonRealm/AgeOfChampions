#include "Buildings.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1EntitiesManager.h"

/// Habitable Building --------------------------
//Class that defines the base for habitable buildings

//Constructors ========================
HabitableBuilding::HabitableBuilding() : Building()
{

}

HabitableBuilding::HabitableBuilding(const HabitableBuilding & copy) : Building(copy), units_capacity(copy.units_capacity), current_units(copy.current_units), units_spawn_point(copy.units_spawn_point)
{

}

//Destructors =========================
HabitableBuilding::~HabitableBuilding()
{

}

//Functionality =======================
//Cover / Release units -
bool HabitableBuilding::CoverUnit(const Unit * target)
{
	if (units_capacity == current_units)
	{
		LOG("Building is full!");
		return false;
	}

	units_in.push_back(App->entities_manager->PopUnit(target));

	return true;
}

void HabitableBuilding::ReleaseUnit(const Unit * target)
{
	units_in.remove((Unit*)target);
	App->entities_manager->AddUnit(target);
}

void HabitableBuilding::ReleaseAllUnits()
{
	uint size = units_in.size();
	for (uint k = 0; k < size; k++)
	{
		App->entities_manager->AddUnit(units_in.back());
		units_in.pop_back();
	}
}

//Set Methods -----------
void HabitableBuilding::SetUnitsCapacity(uint capacity)
{
	units_capacity = capacity;
}

void HabitableBuilding::SetCurrentUnits(uint units)
{
	current_units = units;
}

void HabitableBuilding::SetUnitsSpawnPoint(const iPoint & point)
{
	units_spawn_point = point;
}

//Get Methods -----------
uint HabitableBuilding::GetUnitsCapacity() const
{
	return units_capacity;
}

uint HabitableBuilding::GetCurrentUnits() const
{
	return current_units;
}

iPoint HabitableBuilding::GetSpawnPoint() const
{
	return units_spawn_point;
}

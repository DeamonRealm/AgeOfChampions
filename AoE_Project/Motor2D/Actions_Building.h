#ifndef __ACTION_BUILDING_H__
#define __ACTION_BUILDING_H__

#include "j1ActionManager.h"
#include "j1EntitiesManager.h"
#include "BaseEntities.h"
#include "j1App.h"

class SpawnUnitAction : public Action
{
public:
	
	SpawnUnitAction(HabitableBuilding* actor, UNIT_TYPE type, DIPLOMACY diplomacy) : Action(actor), type(type), diplomacy(diplomacy)
	{
		new_unit = App->entities_manager->GenerateUnit(type, diplomacy, false);
		new_unit->SetDiplomacy(diplomacy);
		time = new_unit->GetTrainTime();
	}
	
	~SpawnUnitAction()
	{
	
	}

	bool execute()
	{
		if (timer.Read() >= time)
		{
			int x = ((HabitableBuilding*)actor)->GetSpawnPoint().x + actor->GetPosition().x;
			int y = ((HabitableBuilding*)actor)->GetSpawnPoint().y + actor->GetPosition().y;
			App->entities_manager->AddUnit(new_unit);
			new_unit->SetPosition(x, y);
			
			return true;
		}

		return false;
	}

private:

	Unit*		new_unit = nullptr;
	uint		time = 0;
	j1Timer		timer;
	UNIT_TYPE	type = UNIT_TYPE::NO_UNIT;
	DIPLOMACY	diplomacy = DIPLOMACY::NEUTRAL;

};
#endif // __ACTION_BUILDING_H__

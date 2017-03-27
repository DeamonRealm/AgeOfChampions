#ifndef __ACTION_BUILDING_H__
#define __ACTION_BUILDING_H__

#include "j1ActionManager.h"
#include "j1EntitiesManager.h"
#include "BaseEntities.h"
#include "j1App.h"

class SpawnUnitAction : public Action
{
public:
	SpawnUnitAction(Building* actor, UNIT_TYPE type) : Action(actor), type(type)
	{
			new_unit = App->entities_manager->GenerateUnit(type, false);
			//timer = new_unit->GetTrainTime();
			time = new_unit->GetTrainTime();
	}
	~SpawnUnitAction()
	{}

	bool execute()
	{
		if (timer.Read() >= time)
		{
			int x = ((Building*)actor)->GetSpawnPoint().x + actor->GetPosition().x;
			int y = ((Building*)actor)->GetSpawnPoint().y + actor->GetPosition().y;

			App->entities_manager->AddUnit(new_unit);
		
			new_unit->SetPosition(x, y);
			new_unit->SetDiplomacy(ALLY);
			return true;
		}
		else
			return false;
	}

private:
	Unit* new_unit = nullptr;
	uint time = 0;
	j1Timer timer;
	UNIT_TYPE type = UNIT_TYPE::NO_UNIT;
};





#endif // __ACTION_BUILDING_H__

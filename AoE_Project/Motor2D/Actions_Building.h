#ifndef __ACTION_BUILDING_H__
#define __ACTION_BUILDING_H__

#include "j1ActionManager.h"
#include "j1EntitiesManager.h"
#include "BaseEntities.h"
#include "j1App.h"
#include "j1SoundManager.h"
#include "Actions_Unit.h"

class SpawnUnitAction : public Action
{
public:
	
	SpawnUnitAction(ProductiveBuilding* actor, UNIT_TYPE type, DIPLOMACY diplomacy) : Action(actor,TASK_B_SPAWN_UNITS), type(type), diplomacy(diplomacy)
	{
		new_unit = App->entities_manager->GenerateUnit(type, diplomacy, false);
		new_unit->SetDiplomacy(diplomacy);
		time = new_unit->GetTrainTime();
	}
	
	~SpawnUnitAction()
	{
	
	}

	bool Activation()
	{
		timer.Start();
		return true;
	}


	bool Execute()
	{
		if (timer.Read() >= time)
		{
			iPoint spawn_point = ((ProductiveBuilding*)actor)->GetSpawnPoint();
			int x = ((ProductiveBuilding*)actor)->GetSpawnPoint().x + actor->GetPosition().x;
			int y = ((ProductiveBuilding*)actor)->GetSpawnPoint().y + actor->GetPosition().y;
			App->entities_manager->AddUnit(new_unit);
			new_unit->SetPosition(x, y);
			App->animator->UnitPlay(new_unit);
			App->sound->PlayFXAudio(SOUND_TYPE::VILLAGER_CREATED_SOUND);

			new_unit->AddAction(App->action_manager->MoveAction(new_unit, iPoint(spawn_point.x + actor->GetPosition().x + 1, spawn_point.y + actor->GetPosition().y + 1)), TASK_CHANNELS::PRIMARY);

			//Add an autoatack passive action
			new_unit->AddAction(App->action_manager->ScanAction(new_unit), TASK_CHANNELS::PASSIVE);

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

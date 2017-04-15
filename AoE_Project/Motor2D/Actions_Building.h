#ifndef __ACTION_BUILDING_H__
#define __ACTION_BUILDING_H__

#include "j1ActionManager.h"
#include "j1EntitiesManager.h"
#include "BaseEntities.h"
#include "j1App.h"
#include "j1SoundManager.h"
#include "Actions_Unit.h"
#include "j1Player.h"

enum RESEARCH_TECH
{
	NO_TECH,

	//Archery Tech
	A_ARCHER_UP,
	A_C_ARCHER_UP,
	A_CROSSBOW_UP,
	A_PARTHIAN_T_UP,
	A_THUMBRING_UP,

	//Barrak Tech
	B_MILITIA_UP,
	B_MAN_AT_ARMS_UP,
	B_LONGSWORDSMAN_UP,
	B_TWO_HANDED_UP,

	//Blacksmith Tech
	BS_BLASTFURNACE,
	BS_BODKINARROW,
	BS_BRACER,
	BS_FLETCHING,
	BS_FORGING,
	BS_IRONCASTING,
	BS_LEATHER_AA,
	BS_PADDED_AA,
	BS_RING_AA,
	BS_CHAIN_BA,
	BS_PLATE_BA,
	BS_SCALE_BA,
	BS_CHAIN_MAIL_ARMOR,
	BS_PLATE_MAIL_ARMOR,
	BS_SCALE_MAIL_ARMOR,

	// Castle Tech
	// LC wood
	// M mill
	// MC stone/gold
	// Mk Market

	// Monk

	// Stable
	S_BLOODLINES,
	S_KNIGHT_UP,
	S_CAVALLIER_UP,
	S_HUSBANDRY,
	S_SCOUTC_UP,

	// Seige Workshop

	// TownCenter Tech
	TC_FEUDAL,
	TC_CASTLE,
	TC_IMPERIAL,
	TC_LOOM,
	TC_PATROL,
	TC_TOWNWATCH,
	TC_WHEELBARROW,

	// University
};

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

class ResearchTecAction : public Action
{
public:
	ResearchTecAction(RESEARCH_TECH type, uint r_time, j1Module* callback, DIPLOMACY diplomacy = ALLY) : Action(nullptr, TASK_B_SPAWN_UNITS), type(type), civilization(callback),diplomacy(diplomacy), research_time(r_time) {};
	~ResearchTecAction() {};

	bool Activation()
	{
		timer.Start();
		return true;
	}

	bool Execute()
	{
		time = timer.Read();
		if (research_time <= time)
		{
			if (civilization == App->player)
			{
				App->entities_manager->UpgradeEntity(type, diplomacy);
				App->player->UpgradeCivilization(type);
			}
			App->sound->PlayFXAudio(BLACKSMITH_SOUND);
			return true;
		}

		return false;
	}
private:
	j1Module*		civilization = nullptr;
	uint			time = 0;
	uint			research_time = 0;
	j1Timer			timer;
	RESEARCH_TECH	type = RESEARCH_TECH::NO_TECH;
	DIPLOMACY		diplomacy = DIPLOMACY::NEUTRAL;
};


#endif // __ACTION_BUILDING_H__

#ifndef __ACTION_BUILDING_H__
#define __ACTION_BUILDING_H__

#include "j1ActionManager.h"
#include "j1EntitiesManager.h"
#include "BaseEntities.h"
#include "j1App.h"
#include "j1SoundManager.h"
#include "Actions_Unit.h"
#include "j1Player.h"
#include "j1AI.h"

enum RESEARCH_TECH
{
	NO_TECH,

	//Archery Tech
	A_ARCHER_UP,
	A_CROSSBOW_UP,
	A_SKIMISHER_UP,
	A_C_ARCHER_UP,
	A_THUMBRING_UP,

	//Barrak Tech
	B_MILITIA_UP,
	B_MAN_AT_ARMS_UP,
	B_LONGSWORDSMAN_UP,
	B_TWO_HANDED_UP,
	B_SPEARMAN_UP,

	//Blacksmith Tech
	BS_PADDED_AA,
	BS_LEATHER_AA,
	BS_RING_AA,
	BS_FLETCHING,
	BS_BODKINARROW,
	BS_BRACER,
	BS_FORGING,
	BS_IRONCASTING,
	BS_BLASTFURNACE,
	BS_SCALE_BA,
	BS_CHAIN_BA,
	BS_PLATE_BA,
	BS_SCALE_MAIL_ARMOR,
	BS_CHAIN_MAIL_ARMOR,
	BS_PLATE_MAIL_ARMOR,

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
	TC_HANDCART,

	// Castle Tech
	// Mill Tecnology
	M_HORSECOLLAR,
	M_HEAVYPLOW,
	M_CROPROTATION,

	// Lumber Camp Tecnology
	LC_DOUBLEBIT_AXE,
	LC_BOW_SAW,
	LC_TWOMAN_SAW,

	// Mining Camp Tecnology
	MC_GOLD_MINING,
	MC_GOLD_SHAFT,
	MC_STONE_MINING,
	MC_STONE_SHAFT

	// Mk Market

	// Monk
	// University
};

class SpawnUnitAction : public Action
{
public:

	SpawnUnitAction(ProductiveBuilding* actor, UNIT_TYPE type, DIPLOMACY diplomacy, uint runned_time = 0) : Action(actor, TASK_B_SPAWN_UNITS), type(type), diplomacy(diplomacy)
	{
		new_unit = App->entities_manager->GenerateUnit(type, diplomacy, false);
		new_unit->SetDiplomacy(diplomacy);
		if (runned_time > new_unit->GetTrainTime())time = 0;
		else time = new_unit->GetTrainTime() - runned_time;
	}

	~SpawnUnitAction()
	{
		if (new_unit != nullptr)
		{
			delete new_unit;
		}
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
			new_unit->SetPosition((float)x, (float)y);
			App->animator->UnitPlay(new_unit);
			if (diplomacy == ALLY)App->sound->PlayFXAudio(SOUND_TYPE::VILLAGER_CREATED_SOUND);

			new_unit->AddAction(App->action_manager->MoveAction(new_unit, iPoint(spawn_point.x + actor->GetPosition().x + 1, spawn_point.y + actor->GetPosition().y + 1)), TASK_CHANNELS::PRIMARY);

			//Add an autoattack passive action
			new_unit->AddAction(App->action_manager->ScanAction(new_unit), TASK_CHANNELS::PASSIVE);


			//Add it to the AI units list if it is an enemy
			if (new_unit->GetDiplomacy() == ENEMY)
				App->AI->enemy_units.push_back(new_unit);

			new_unit = nullptr;
			return true;
		}

		return false;
	}

	//Get Methods -----------
	DIPLOMACY GetUnitDiplomacy()const
	{
		return diplomacy;
	}

	UNIT_TYPE GetUnitType()const
	{
		return type;
	}

	uint GetTime()const
	{
		return timer.Read();
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
	ResearchTecAction(RESEARCH_TECH type, uint r_time, DIPLOMACY diplomacy = ALLY) : Action(nullptr, TASK_B_RESEARCH), type(type), diplomacy(diplomacy), research_time(r_time) {};
	~ResearchTecAction() {};

	bool Activation()
	{
		timer.Start();
		return true;
	}

	bool Execute()
	{
		if (research_time <= timer.Read())
		{
			if (diplomacy == ALLY)
			{
				App->entities_manager->UpgradeEntity(type, diplomacy);
				App->player->UpgradeCivilization(type);
			}
			App->sound->PlayFXAudio(BLACKSMITH_SOUND);
			return true;
		}

		return false;
	}

	//Get Methods -----------
	RESEARCH_TECH GetResearchType()const
	{
		return type;
	}

	DIPLOMACY GetDiplomacy()const
	{
		return diplomacy;
	}

	uint GetTime()const
	{
		return timer.Read();
	}

	uint GetResearchTime()const
	{
		return research_time;
	}

private:

	uint			research_time = 0;
	j1Timer			timer;
	RESEARCH_TECH	type = RESEARCH_TECH::NO_TECH;
	DIPLOMACY		diplomacy = DIPLOMACY::NEUTRAL;
};


#endif // __ACTION_BUILDING_H__

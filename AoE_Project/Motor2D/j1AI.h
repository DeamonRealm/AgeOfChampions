#ifndef _J1_AI_H_
#define _J1_AI_H_

#include "j1Module.h"
#include "p2Point.h"
#include "p2Log.h"
#include <list>

#include "BaseEntities.h"

class ActionWorker;

enum PLAYER_RESOURCES;
enum RESEARCH_TECH;

#define	RESEARCH_RATE 15000
#define AI_RESEARCH_DURATION 10000
#define MIN_VILLAGERS 6
#define PRODUCTIVE_SIZE 22


///AI is controlled thorugh childs from this command (like ActionManager)--------
class AICommand
{
public:
	AICommand() {};
	~AICommand() {};
public:
	//Returns false if Action was unable to initialize
	virtual bool Activation() { return true; }
	//Returns TRUE when execute finshed
	virtual bool Execute() { return true; };

};
///--------------------------------------


///AI Worker-----------------------------
class AIWorker
{
public:
	AIWorker();
	~AIWorker();

private:
	std::list<AICommand*> command_queue;
	AICommand* current_command = nullptr;


public:
	void Update();
	void AddAICommand(AICommand* command);
	void Reset();

};
///------------------------------------


///Entities Data-----------------------
struct AI_Entities_Data
{
	// Data
	UNIT_TYPE		base_type_unit = ((UNIT_TYPE)0);
	BUILDING_TYPE	spawn_from = ((BUILDING_TYPE)0);
	BUILDING_TYPE	spawn_from2 = ((BUILDING_TYPE)0);
	uint			cell_at_level = 0;
	uint			cell_at_age = 0;

	// Data
	int				wood_cost = 0;
	int				food_cost = 0;
	int				gold_cost = 0;
	int				stone_cost = 0;
	int				population_cost = 0;

	UNIT_TYPE		u_type = ((UNIT_TYPE)0);
	RESEARCH_TECH	r_type = ((RESEARCH_TECH)0);
};

struct AI_Entities_Data_Buildings
{
	// Data
	BUILDING_TYPE	base_type_building = (BUILDING_TYPE)0;
	UNIT_TYPE		spawn_from = ((UNIT_TYPE)0);
	uint			cell_at_level = 0;
	uint			cell_at_age = 0;

	// Data
	int				wood_cost = 0;
	int				food_cost = 0;
	int				gold_cost = 0;
	int				stone_cost = 0;
	int				population_cost = 0;

	BUILDING_TYPE	b_type = ((BUILDING_TYPE)0);
	RESEARCH_TECH	r_type = ((RESEARCH_TECH)0);
};
///------------------------------------


class j1AI : public j1Module
{
public:
	j1AI();
	~j1AI();

public:
	void Init() { active = false; };

	void Enable();
	void Disable();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	//Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	// Save and Load
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	void GenerateDebugVillager();
	bool GenerateUnit(UNIT_TYPE type);
	Building* GenerateBuilding(BUILDING_TYPE type);

	void AddResources(PLAYER_RESOURCES type, int value);
	bool CheckResources(int amount_wood = 0, int amount_food = 0, int amount_gold = 0, int amount_stone = 0, int used_population = 0, ENTITY_TYPE type = NO_ENTITY, bool use = true);
	
	// Upgrade Research
	void UpgradeCivilization(RESEARCH_TECH type);


private:

	Resource*	GetNearestNeed(iPoint pos);

	void ManageAttack();
	void ManageConstrucion();

	void ManageTroopsCreation();

	// AI Entity Data Methods
	void LoadAIEntitiesData(pugi::xml_node&);

	void UpdateResearch();
	void UpdateAIUnits();
	void UpdateAIBuildings();

	void GetSpawnUnitTypes(UNIT_TYPE& u_type);
	UNIT_TYPE  GetNextSpawnType(UNIT_TYPE u_type);

	Building* FindBuilding(BUILDING_TYPE type, BUILDING_TYPE type2);

	BUILDING_TYPE GetNextBuildingSpawnType();

private:
	uint current_age = 2;

	AIWorker* ai_worker = nullptr;
	j1Timer update_timer;
	j1Timer building_timer;


	//ProductiveBuilding* ai_starting_tc;
	std::list<Building*> enemy_buildings;
	std::list<Unit*> enemy_raid;

	// Reserach
	ActionWorker*	ai_research_worker = nullptr;
	std::vector<AI_Entities_Data> research_queue;
	uint		next_research = 0;
	j1Timer		research_timer;

	// Units and Buildings Data
	std::vector<AI_Entities_Data> units_queue;
	std::vector<AI_Entities_Data_Buildings> buildings_queue;

	// Aviable Units/Buildings to create
	std::vector<AI_Entities_Data> units_production;
	int							  units_lvl[PRODUCTIVE_SIZE];
	std::vector<AI_Entities_Data_Buildings> buildings_production;
	int							  buildings_lvl[PRODUCTIVE_SIZE];

public:
	std::list<Unit*> enemy_units;

	//AI resource information
	int			wood = 50000;
	int			research_wood = 50000;
	int			meat = 50000;
	int			research_meat = 50000;
	int			gold = 50000;
	int			research_gold = 50000;
	int			stone = 50000;
	int			research_stone = 50000;
	int			population = 0;
	int			max_population = 200;
	

};


///AI Commands-------------------------
//Wait command--------
class WaitAICommand : public AICommand
{
public:
	WaitAICommand(uint time);
	~WaitAICommand();

public:
	bool Activation();
	bool Execute();


private:
	uint time = 0;
	j1Timer timer;

};
//---------------------

//Spawn from list------
class SpawnUnitsFromListCommand : public AICommand
{
public:
	SpawnUnitsFromListCommand(std::list<Unit*>* to_spawn);
	~SpawnUnitsFromListCommand();

public:
	bool Execute();

private:
	std::list<Unit*>* to_spawn;
	Unit* current_spawn = nullptr;

};
//---------------------


//Spawn x unit---------
class SpawnUnitCommand : public AICommand
{
public:
	SpawnUnitCommand(UNIT_TYPE type, ProductiveBuilding* generator);
	~SpawnUnitCommand();

public:
	bool Execute();


private:
	UNIT_TYPE type = UNIT_TYPE::NO_UNIT;
	ProductiveBuilding* generator = nullptr;
};
//---------------------





//---------------------
class SendToRecollect : public AICommand
{
public:
	SendToRecollect(std::list<Unit*> units, Resource* target);
	~SendToRecollect();

public:

	bool Execute();
	
private:

	std::list<Unit*> units;
	Resource* target = nullptr;
};
//---------------------

class MoveUnitsCommand : public AICommand
{
public:
	MoveUnitsCommand(std::list<Entity*>& to_move_list, iPoint new_pos);
	~MoveUnitsCommand();


public:
	bool Execute();


private:
	std::list<Entity*>& to_move_list;
	iPoint new_pos = { 0,0 };

};


#endif // !_J1_AI_H_


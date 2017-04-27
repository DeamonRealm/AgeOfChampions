#ifndef _J1_AI_H_
#define _J1_AI_H_

#include "j1Module.h"
#include "p2Point.h"
#include "p2Log.h"
#include <list>

#include "BaseEntities.h"


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

private:

	bool LoadEnemies(const char* folder);



private:
	AIWorker* ai_worker = nullptr;
	std::list<Entity*> units_to_spawn;
	j1Timer update_timer;

	ProductiveBuilding* ai_starting_tc;



public:
	std::list<Unit*> enemy_units;
	
	//AI resource information
	int			wood = 0;
	int			meat = 0;
	int			gold = 0;
	int			stone = 0;
	int			population = 0;

	void		GenerateDebugVillager();


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
class BuildStructureCommand : public AICommand
{
public:
	BuildStructureCommand();
	~BuildStructureCommand();

public:
	bool Execute();


private:


};
//---------------------


//---------------------
class SendToRecollect : public AICommand
{
public:
	SendToRecollect(std::list<Unit*> units, Resource** target);
	~SendToRecollect();

public:
	bool Execute();


private:

	std::list<Unit*> units;
	Resource** target;	
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
	iPoint new_pos;

};


#endif // !_J1_AI_H_


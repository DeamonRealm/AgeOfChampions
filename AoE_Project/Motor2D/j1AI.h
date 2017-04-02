#ifndef _J1_AI_H_
#define _J1_AI_H_

#include "j1Module.h"
#include "p2Log.h"

class Unit;

class j1AI : public j1Module
{
public:
	j1AI();
	~j1AI();

public:
	void Init() { active = false; };

	void Enable();

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
	std::list<Unit*> enemy_units;

};


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
	virtual bool execute() { return true; };

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


///AI Commands-------------------------
//Wait command
class WaitAICommand
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

#endif // !_J1_AI_H_


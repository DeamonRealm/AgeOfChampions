#ifndef _J1_BUFF_MANAGER_H_
#define _J1_BUFF_MANAGER_H_


#include "j1Module.h"
#include "j1Timer.h"

class Unit;

enum BUFF_TYPE
{
	NO_BUFF = 0,
	ATTACK_BUFF,
	DEFENSE_BUFF
};

class Buff
{
public:

	Buff();

	Buff(const Buff& copy);

	~Buff();

private:
	
	BUFF_TYPE	type = NO_BUFF;
	float		value = 0;
	j1Timer		timer;
	uint		buff_duration = 0;
	Unit*		target = nullptr;

public:

	//setter
	void SetType(BUFF_TYPE type);
	void SetValue(float value);
	void SetBuffDuration(uint buff_duration);
	void SetTarget(Unit* target);

	//getters
	BUFF_TYPE	GetBuffType()const;
	float		GetValue()const;
	uint		GetBuffDuration()const;
	Unit*		GetTarget()const;

	//Utils

	bool ApplyBuff();
	bool RemoveBuff(); //remove buff from target when time it's over.
	bool IsActive()const;
};

class j1BuffManager: public j1Module
{
public:

	j1BuffManager();

	~j1BuffManager();

	// Called before the first frame
	virtual bool Start();

	// Called each loop iteration
	virtual bool Update(float dt);

	// Called before quitting
	virtual bool CleanUp();

	bool CallBuff(Unit* target, BUFF_TYPE type);


private:

	std::vector<Buff*> buff_definitions;
	std::list<Buff*> active_buffs;

};








#endif //_J1_BUFF_MANAGER_H_

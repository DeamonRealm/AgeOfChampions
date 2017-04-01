#ifndef _J1_BUFF_MANAGER_H_
#define _J1_BUFF_MANAGER_H_


#include "j1Module.h"
#include "j1Timer.h"
#include "p2Point.h"
#include "j1Animator.h"

class Unit;

enum BUFF_TYPE
{
	NO_BUFF = 0,
	PASSIVE_BUFF,
	TIMED_BUFF
};

enum BUFF_ATTRIBUTE_TYPE
{
	NO_ATTRIBUTE = 0,
	ATTACK_BUFF,
	DEFENSE_BUFF
};

/// Class Buff Particle ---------------
class Particle
{
public:

	Particle();
	Particle(const Particle& copy);
	~Particle();

public:

	iPoint				position = { 0,0 };
	bool				actor = false;
	BUFF_TYPE			buff_type = NO_BUFF;
	BUFF_ATTRIBUTE_TYPE	attribute_type = NO_ATTRIBUTE;
	j1Timer				draw_timer;
	uint				draw_rate = 0;
	Animation			animation;

public:

	void Draw();

};
/// -----------------------------------

/// Class Passive Buff ----------------
class PassiveBuff
{
public:

	PassiveBuff();
	PassiveBuff(const PassiveBuff& copy);
	~PassiveBuff();

protected:

	BUFF_TYPE			buff_type = NO_BUFF;
	BUFF_ATTRIBUTE_TYPE	attribute_type = NO_ATTRIBUTE;
	float				value = 0;
	Unit*				target = nullptr;
	bool				actor = false;
	Particle			particle;

public:

	void Draw();

	//setter
	void SetBuffType(BUFF_TYPE type);
	void SetAttributeType(BUFF_ATTRIBUTE_TYPE type);
	void SetValue(float value);
	void SetTarget(Unit* target);
	void SetActor(bool act);
	void SetParticle(Particle part);

	//getters
	BUFF_TYPE			GetBuffType()const;
	BUFF_ATTRIBUTE_TYPE	GetAttributeType()const;
	float				GetValue()const;
	Unit*				GetTarget()const;
	bool				GetActor()const;

	//Utils
	virtual bool ApplyBuff();
	virtual bool RemoveBuff(); //remove buff from target when time it's over.

	//Compare operator
	bool operator == (const PassiveBuff& tar)const;

};
/// -----------------------------------

/// Class Buff ------------------------
class Buff : public PassiveBuff
{
public:

	Buff();
	Buff(const Buff& copy);
	~Buff();

private:
	
	j1Timer		timer;
	uint		buff_duration = 0;

public:

	//setter ----------------
	void SetBuffDuration(uint buff_duration);

	//getters ---------------
	uint GetBuffDuration()const;

	//Functionality ---------
	bool ApplyBuff();
	bool IsActive()const;

};
/// -----------------------------------


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



private:

	//Methods to transform loaded string to buffs enums
	BUFF_TYPE			StrToBuffType(const char* str)const;
	BUFF_ATTRIBUTE_TYPE StrToBuffAttributeType(const char* str)const;

	std::vector<PassiveBuff*>	buff_definitions;
	std::vector<Particle*>		buff_particle_definitions;
	std::list<Buff*>			active_buffs;
	std::list<PassiveBuff*>		static_buffs;

	//Get a buff particle looking the types and the actor
	Particle GetBuffParticle(BUFF_TYPE bf_type, BUFF_ATTRIBUTE_TYPE atr_type, bool act);

public:

	bool			CallBuff(Unit* target, BUFF_TYPE buff_type, BUFF_ATTRIBUTE_TYPE buff_atr_type, bool actor = false);
	PassiveBuff*	GetPassiveBuff(BUFF_TYPE buff_type, BUFF_ATTRIBUTE_TYPE buff_atr_type, bool actor);
	void			RemoveTargetBuff(Unit* target, PassiveBuff* buff);

};
#endif //_J1_BUFF_MANAGER_H_

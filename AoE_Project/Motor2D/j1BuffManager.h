#ifndef _J1_BUFF_MANAGER_H_
#define _J1_BUFF_MANAGER_H_


#include "j1Module.h"
#include "j1Timer.h"
#include "p2Point.h"
#include "j1Animator.h"
#include "Units.h"

enum PARTICLE_TYPE
{
	UNKNOWN_PARTICLE,
	BUFF_PARTICLE,
	SLASH_PARTICLE,
	STUN_PARTICLE,
	TELEPORT_PARTICLE,
	HEAL_PARTICLE,
	TAUNT_PARTICLE,
	THUNDER_PARTICLE,
	MULTI_SHOT_PARTICLE,
	DRAGON_SHOT_PARTICLE,
	RESURRECTION_PARTICLE,
	LEVEL_UP_PARTICLE,
	ONE_HIT_PARTICLE,
	LASER_PARTICLE,
	LONG_SHOT_PARTICLE,
	RADIUS_HIT_SMALL_PARTICLE,
	RADIUS_HIT_BIG_PARTICLE

};

enum BUFF_TYPE
{
	NO_BUFF = 0,
	PASSIVE_BUFF,
	TIMED_BUFF
};

enum BUFF_ATTRIBUTE_TYPE
{
	NO_ATTRIBUTE = 0,
	VISION_BUFF,
	LIFE_BUFF,
	ATTACK_BUFF,
	SUPER_ATTACK_BUFF,
	DEFENSE_BUFF,
	STUN_BUFF,
	TAUNT_BUFF,
	SPEED_BUFF,
	RANGE_BUFF,
	BURN_BUFF
};

///Class Particle ---------------------
class Particle
{
public:

	Particle();
	Particle(const Particle& copy);
	~Particle();

public:

	PARTICLE_TYPE	particle_type = UNKNOWN_PARTICLE;
	DIRECTION_TYPE	direction_type = NO_DIRECTION;
	iPoint			position = { 0,0 };
	int				z_depth = 0;
	Animation		animation;
	bool			flip_sprite = false;
	SDL_Color		blit_color = { 255,255,255,255 };

public:
	void			SetBlitColor(const SDL_Color new_color);
	virtual void Draw(double angle =0.0);

};
/// -----------------------------------

/// Class Buff Particle ---------------
class BuffParticle : public Particle
{
public:

	BuffParticle();
	BuffParticle(const BuffParticle& copy);
	~BuffParticle();

public:

	bool				actor = false;
	BUFF_TYPE			buff_type = NO_BUFF;
	BUFF_ATTRIBUTE_TYPE	attribute_type = NO_ATTRIBUTE;
	j1Timer				draw_timer;
	uint				draw_rate = 0;

public:

	void Draw(double angle = 0.0);

};
/// -----------------------------------

/// Class Passive Buff ----------------
class PassiveBuff
{
public:

	PassiveBuff();
	PassiveBuff(const PassiveBuff& copy);
	~PassiveBuff();

public:

	BUFF_TYPE			buff_type = NO_BUFF;
	BUFF_ATTRIBUTE_TYPE	attribute_type = NO_ATTRIBUTE;
	float				value = 0;
	Unit*				target = nullptr;
	bool				actor = false;
	BuffParticle		particle;

public:

	void Draw(double angle = 0.0);

	//setter
	void SetBuffType(BUFF_TYPE type);
	void SetAttributeType(BUFF_ATTRIBUTE_TYPE type);
	void SetValue(float value);
	void SetTarget(Unit* target);
	void SetActor(bool act);
	void SetParticle(BuffParticle part);

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
	
	j1Timer			timer;
	uint			buff_duration = 0;


public:

	//setter ----------------
	void SetBuffDuration(uint buff_duration);

	//getters ---------------
	uint GetBuffDuration()const;

	//Functionality ---------
	bool ApplyBuff();
	bool IsActive()const;
	void Reset();
};
/// -----------------------------------


class j1BuffManager: public j1Module
{
public:

	j1BuffManager();

	~j1BuffManager();

	void Init();
	bool Enable();
	void Disable();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();



private:

	//Methods to transform loaded string to buffs enums
	BUFF_TYPE			StrToBuffType(const char* str)const;
	BUFF_ATTRIBUTE_TYPE StrToBuffAttributeType(const char* str)const;
	PARTICLE_TYPE		StrToParticleType(const char* str)const;

	std::vector<PassiveBuff*>	buff_definitions;
	std::vector<BuffParticle*>	buff_particle_definitions;
	std::vector<Particle*>		particle_definitions;

	std::list<Buff*>			active_buffs;
	std::list<PassiveBuff*>		static_buffs;

	//Get a buff particle looking the types and the actor
	BuffParticle	GetBuffParticle(BUFF_TYPE bf_type, BUFF_ATTRIBUTE_TYPE atr_type, bool act);

	//Data use to segment the buffs load process
	pugi::xml_document buffs_data;
	bool buffs_data_loaded = false;
	pugi::xml_node particle_document_node;
	pugi::xml_node buff_node;
	bool load_step = false;

public:

	bool			CheckBuff(Unit* target, BUFF_TYPE buff_type, BUFF_ATTRIBUTE_TYPE buff_atr_type);
	bool			CallBuff(Unit* target, BUFF_TYPE buff_type, BUFF_ATTRIBUTE_TYPE buff_atr_type, bool actor = false);
	PassiveBuff*	GetPassiveBuff(BUFF_TYPE buff_type, BUFF_ATTRIBUTE_TYPE buff_atr_type, bool actor);
	void			RemoveTargetBuff(Unit* target, PassiveBuff* buff);
	void			RemoveTargetBuffs(Unit* target);
	Particle		GetParticle(PARTICLE_TYPE ty, DIRECTION_TYPE direc);

};
#endif //_J1_BUFF_MANAGER_H_

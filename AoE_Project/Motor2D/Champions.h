#ifndef _CHAMPIONS_
#define _CHAMPIONS_

#include "Units.h"
#include "j1BuffManager.h"
#define MAX_LEVEL 2
///Class Champion -------------------------------
//Base class that define the champions bases
class Champion : public Unit
{
public:

	Champion();
	Champion(const Champion& copy);
	~Champion();

protected:

	//Hero level
	uint		level = 2;
	Particle	level_up_particle;
	uint		current_experience = 0;
	uint		experience_lvl_2 = 0;
	uint		experience_lvl_3 = 0;
	bool		to_level_up = false;
	bool		level_up_animation = false;

	//Buff area
	Circle			buff_area;
	PassiveBuff*	buff_to_apply = nullptr;

	//ability area 
	bool	ability[3];
	bool	actived[3];
	bool	skill_choosed[3];

	bool	ability_lvl_2_prepare_mode = false;
	bool	ability_lvl_3_prepare_mode = false;
	//Attack area lvl 2
	j1Timer	ability_lvl_2_timer;
	uint	ability_lvl_2_cooldown = 0;
	uint	ability_lvl_2_current_time = 0;
	bool	ability_lvl_2_ready = false;
	//Attack area lvl 3
	j1Timer	ability_lvl_3_timer;
	uint	ability_lvl_3_cooldown = 0;
	uint	ability_lvl_3_current_time = 0;
	bool	ability_lvl_3_ready = false;

	//Stats bonus for level
	uint	attack_for_level = 0;
	uint	life_for_level = 0;

	uint	range_for_level = 0;
	float	defense_for_level = 0;
	float	speed_for_level = 0;
	uint	view_area_for_level = 0;

	//List of all units buffed by the hero
	std::list<Unit*>	buffed_units;

public:

	//Functionality ---------
	virtual bool Die();
	//Actions -----
	void		CleanBuffedUnits();
	void		CheckCooldown();
	//Ability lvl1 methods
	virtual void SetAbility_lvl_1(bool choosed);
	virtual void Hability_lvl_1();
	virtual void CheckHability_lvl_1();
	//Ability lvl2 methods
	virtual void SetAbility_lvl_2(bool choosed);
	virtual bool PrepareAbility_lvl_2();
	virtual void Hability_lvl_2(int x = 0, int y = 0);
	virtual void CheckHability_lvl_2();
	//Ability lvl3 methods
	virtual void SetAbility_lvl_3(bool choosed);
	virtual bool PrepareAbility_lvl_3();
	virtual void Hability_lvl_3(int x = 0, int y = 0);
	virtual void CheckHability_lvl_3();
	//Level system methods
	void	LevelUp(bool particle = true);
	void	LevelUpAnimation();

	//Set Methods -
	void	SetLevelUpParticle();
	void	SetPosition(float x, float y, bool insert = true);
	void	SetBuffArea(const Circle& area);
	void	SetBuffToApply(const PassiveBuff* buff);
	void	SetLvl2Cap(uint value);
	void	SetLvl3Cap(uint value);
	void	SetAbility_lvl_2_Cooldown(uint value);
	void	SetAbility_lvl_3_Cooldown(uint value);
	void	SetLevel(uint lvl);
	void	SetAttackForLevel(uint atk_for_lvl);
	void	SetRangeForLevel(uint rng_for_lvl);
	void	SetDefenseForLevel(float def_for_lvl);
	void	SetSpeedForLevel(float spd_for_lvl);
	void	SetViewAreaForLevel(uint view_for_level);
	void	SetExperience(int exp);
	void	SetSkillChoosed(bool val, uint index);

	//Get Methods -
	Circle			GetBuffArea()const;
	PassiveBuff*	GetBuffToApply()const;
	uint			GetLevel()const;
	uint			GetAttackForLevel()const;
	uint			GetRangeForLevel()const;
	float			GetDefenseForLevel()const;
	float			GetSpeedForLevel()const;
	uint			GetViewAreaForLevel()const;
	bool			GetAbilityChosen(uint index)const;
	bool			GetSkillChoosed(uint index)const;

};
/// ---------------------------------------------


/// Class Warrior -------------------------------
// Class that defines the warrior champion states
class Warrior : public Champion
{
public:

	Warrior();
	Warrior(const Warrior& copy);
	~Warrior();

private:

	/* extra data */
	std::vector<Unit*>	protected_units;


	Triangle			special_attack_area;
	Circle				area_ability_lvl_3;

	Particle			ability_lvl_2_particle;
	Particle			ability_lvl_3_particle;

	uint				ability_lvl_2_attack_value = 0;
	uint				ability_lvl_2_stun_value = 0;
	uint				ability_lvl_3_attack_value = 0;

public:

	//Functionality ---------
	void ClearProtectedUnits();
	//Save/Load methods
	void	SaveAsDef(pugi::xml_node& node);
	//Game Loop ---
	bool Update();
	//Draw --------
	bool	Draw(bool debug);
	//Actions -----
	void	SetAbility_lvl_1(bool choosed);
	void	Hability_lvl_1();
	void	CheckHability_lvl_1();
	void	SetAbility_lvl_2(bool choosed);
	bool	PrepareAbility_lvl_2();
	void	Hability_lvl_2(int x = 0, int y = 0);
	void	CheckHability_lvl_2();

	void	SetAbility_lvl_3(bool choosed);
	bool	PrepareAbility_lvl_3();
	void	Hability_lvl_3(int x = 0, int y = 0);
	void	CheckHability_lvl_3();
	iPoint	GetiPointFromDirection(DIRECTION_TYPE direction)const;
	void	CalculateSpecialAttackArea(const iPoint& base);

	//Set Methods -
	void	SetPosition(float x, float y, bool insert = true);
	void	SetSpecialAttackArea(const Triangle& tri);
	void	SetAreaAbilityLvl3(const Circle& circle);
	void	SetAbility_lvl_2_AttackValue(uint atk);
	void	SetAbility_lvl_2_StunValue(uint stun);


};
/// ---------------------------------------------

/// Class Wizard -------------------------------
// Class that defines the warrior champion states
class Wizard : public Champion
{
public:

	Wizard();
	Wizard(const Wizard& copy);
	~Wizard();

private:

	/* extra data */
	Circle				area_attack_spell_2;
	Circle				area_attack_spell_3;

	int					area_limit_spell_2;
	int					area_limit_spell_3;

	Particle			ability_lvl_2_A_particle;
	Particle			ability_lvl_2_B_particle;
	Particle			ability_lvl_3_particle;

	uint				ability_lvl_2_heal_value = 0;

	uint				ability_lvl_3_attack_value = 0;


public:

	//Functionality ---------
	//Save/Load methods
	void	SaveAsDef(pugi::xml_node& node);
	//Game Loop ---
	bool Update();
	//Draw --------
	bool	Draw(bool debug);
	//Actions -----
	void	SetAbility_lvl_1(bool choosed);
	void	Hability_lvl_1();
	void	CheckHability_lvl_1();
	void	SetAbility_lvl_2(bool choosed);
	bool	PrepareAbility_lvl_2();
	void	Hability_lvl_2(int x = 0, int y = 0);
	void	CheckHability_lvl_2();
	void	SetAbility_lvl_3(bool choosed);
	bool	PrepareAbility_lvl_3();
	void	Hability_lvl_3(int x = 0, int y = 0);
	void	CheckHability_lvl_3();
	iPoint	GetiPointFromDirection(DIRECTION_TYPE direction)const;
	bool	CalculateSpecialAttackArea(const iPoint& base, bool attack_lvl_2);

	//Set Methods -
	void	SetPosition(float x, float y, bool insert = true);
	void	SetSpecialAttackArea(const Circle& circle, const char* name);
	void	SetAbility_lvl_2_HealValue(uint heal);
	void	SetAbility_lvl_3_AttackValue(uint attack);
	void	SetAreaLimitLvl2(int limit);
	void	SetAreaLimitLvl3(int limit);

};
/// ---------------------------------------------

/// Class Hunter -------------------------------
// Class that defines the hunter champion states
class Hunter : public Champion
{
public:

	Hunter();
	Hunter(const Hunter& copy);
	~Hunter();

private:

	/* extra data */
	//Skill lvl 2 A
	Triangle			area_triangle_attack_skill_A_lvl_2;
	uint				ability_lvl_2_skill_A_attack_value = 0;
	//Skill lvl 2 B
	Circle				area_attack_skill_B_lvl_2;
	int					area_limit_skill_B_lvl_2;
	uint				ability_lvl_2_skill_B_attack_value = 0;
	//Skill lvl 3 A
	Circle				area_attack_skill_A_lvl_3;
	int					area_limit_skill_A_lvl_3;
	uint				ability_lvl_3_skill_A_attack_value = 0;
	//Skill lvl 3 B
	PivotedRect			area_attack_skill_B_lvl_3;
	uint				ability_lvl_3_skill_B_attack_value = 0;


	//skill selected
	uint				ability_lvl_2_attack_value;
	uint				ability_lvl_3_attack_value;

	Particle			ability_lvl_2_particle;
	Particle			ability_lvl_3_particle;




public:

	//Functionality ---------
	//Save/Load methods
	void	SaveAsDef(pugi::xml_node& node);
	//Game Loop ---
	bool Update();
	//Draw --------
	bool	Draw(bool debug);
	//Actions -----
	void	SetAbility_lvl_1(bool choosed);
	void	Hability_lvl_1();
	void	CheckHability_lvl_1();
	void	SetAbility_lvl_2(bool choosed);
	bool	PrepareAbility_lvl_2();
	void	Hability_lvl_2(int x = 0, int y = 0);
	void	CheckHability_lvl_2();
	void	SetAbility_lvl_3(bool choosed);
	bool	PrepareAbility_lvl_3();
	void	Hability_lvl_3(int x = 0, int y = 0);
	void	CheckHability_lvl_3();
	iPoint	GetiPointFromDirection(DIRECTION_TYPE direction)const;
	void	CalculateTriangleAttackArea(const iPoint& base);
	void	CalculateArrorwAttackArea(const iPoint& base);

	bool	CalculateSpecialAttackArea(const iPoint& base, bool attack_lvl_2);

	//Set Methods -
	void	SetPosition(float x, float y, bool insert = true);
	void	SetSpecialAttackArea(const Circle& circle, const char* name);
	void	SetSpecialAttackArea(const Triangle& triangle);
	void	SetSpecialAttackArea(const PivotedRect& rect);

	void	SetAbility_lvl_2_A_AttackValue(uint attack);
	void	SetAbility_lvl_2_B_AttackValue(uint attack);
	void	SetAbility_lvl_3_A_AttackValue(uint attack);
	void	SetAbility_lvl_3_B_AttackValue(uint attack);

	void	SetAreaLimitLvl2(uint limit);
	void	SetAreaLimitLvl3(uint limit);

};
/// ---------------------------------------------
#endif // _CHAMPIONS_


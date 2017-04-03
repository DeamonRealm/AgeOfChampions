#ifndef _CHAMPIONS_
#define _CHAMPIONS_

#include "Units.h"
#include "j1BuffManager.h"

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
	uint	level = 2;
	bool	ability[3];
	bool	actived[3];

	//Buff area
	Circle			buff_area;
	PassiveBuff*	buff_to_apply = nullptr;

	//Stats bonus for level
	uint	attack_for_level = 0;
	uint	range_for_level = 0;
	float	defense_for_level = 0;
	float	armor_for_level = 0;
	float	speed_for_level = 0;
	uint	view_area_for_level = 0;

	//List of all units buffed by the hero
	std::list<Unit*>	buffed_units;

public:

	//Functionality ---------
	//Actions -----
	void		 CleanBuffedUnits();
	//Ability A methods
	virtual void SetAbility_A(bool choosed);
	virtual void Hability_A();
	virtual void CheckHability_A();
	//Ability B methods
	virtual void SetAbility_B(bool choosed);
	virtual void Hability_B();
	virtual void CheckHability_B();

	//Set Methods -
	void	SetPosition(float x, float y, bool insert = true);
	void	SetBuffArea(const Circle& area);
	void	SetBuffToApply(const PassiveBuff* buff);
	void	SetLevel(uint lvl);
	void	SetAttackForLevel(uint atk_for_lvl);
	void	SetRangeForLevel(uint rng_for_lvl);
	void	SetDefenseForLevel(float def_for_lvl);
	void	SetArmorForLevel(float arm_for_lvl);
	void	SetSpeedForLevel(float spd_for_lvl);
	void	SetViewAreaForLevel(uint view_for_level);

	//Get Methods -
	Circle			GetBuffArea()const;
	PassiveBuff*	GetBuffToApply()const;
	uint			GetLevel()const;
	uint			GetAttackForLevel()const;
	uint			GetRangeForLevel()const;
	float			GetDefenseForLevel()const;
	float			GetArmorForLevel()const;
	float			GetSpeedForLevel()const;
	uint			GetViewAreaForLevel()const;

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
	Triangle			special_attack_area;
	Particle			ability_B_particle;
	uint				ability_B_value = 0;

public:

	//Functionality ---------
	//Game Loop ---
	bool Update();
	//Draw --------
	bool	Draw(bool debug);
	//Actions -----
	void	SetAbility_A(bool choosed);
	void	Hability_A();
	void	CheckHability_A();
	void	SetAbility_B(bool choosed);
	void	Hability_B();
	void	CheckHability_B();
	void	CalculateSpecialAttackArea(const iPoint& base);

	//Set Methods -
	void	SetPosition(float x, float y, bool insert = true);
	void	SetSpecialAttackArea(const Triangle& tri);


};
/// ---------------------------------------------
#endif // _CHAMPIONS_


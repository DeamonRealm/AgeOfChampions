#ifndef _CHAMPIONS_
#define _CHAMPIONS_

#include "Units.h"

///Class Champion -------------------------------
//Base class that define the champions bases
class Champion : public Unit
{
public:

	Champion();
	Champion(const Champion& copy);
	~Champion();

protected:

	uint level = 1;
	uint attack_for_level = 0;
	uint range_for_level = 0;
	float defence_for_level = 0;
	float armor_for_level = 0;
	float speed_for_level = 0;
	uint  view_area_for_level = 0;

public:

	//Functionality ---------
	//Actions -----
	virtual void Hability_A(const iPoint zone);
	virtual void Hability_B(const iPoint zone);

	//Set Methods -
	void	SetLevel(uint lvl);
	void	SetAttackForLevel(uint atk_for_lvl);
	void	SetRangeForLevel(uint rng_for_lvl);
	void	SetDefenceForLevel(float def_for_lvl);
	void	SetArmorForLevel(float arm_for_lvl);
	void	SetSpeedForLevel(float spd_for_lvl);
	void	SetViewAreaForLevel(uint view_for_level);

	//Get Methods -
	uint	GetLevel()const;
	uint	GetAttackForLevel()const;
	uint	GetRangeForLevel()const;
	float	GetDefenceForLevel()const;
	float	GetArmorForLevel()const;
	float	GetSpeedForLevel()const;
	uint	GetViewAreaForLevel()const;

};
/// ---------------------------------------------


/// Class Warrior -------------------------------
class Warrior : public Unit
{
public:

	Warrior();
	Warrior(const Warrior& copy);
	~Warrior();

private:

	/* extra data */
	float buff_percent = 0; /*This value is in % per cent*/
	uint stun_range = 0;
	uint stun_time = 0;
	uint hability_A_cooldown = 0;
	uint hability_B_cooldown = 0;

public:

	//Functionality ---------
	//Actions -----
	void Hability_A(const iPoint zone);
	void Hability_B(const iPoint zone);


};
/// ---------------------------------------------
#endif // _CHAMPIONS_


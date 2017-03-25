#include "Champions.h"

#include "j1App.h"
#include "j1Animator.h"
#include "j1Render.h"

#include "j1Input.h" /*This is temporal*/

///Class Champion -------------------------------
//Base class that define the champions bases

//Constructors ========================
Champion::Champion() : Unit()
{

}

Champion::Champion(const Champion & copy) : Unit(copy), level(copy.level), attack_for_level(copy.attack_for_level), range_for_level(copy.range_for_level),
defense_for_level(copy.defense_for_level), armor_for_level(copy.armor_for_level), speed_for_level(copy.speed_for_level), view_area_for_level(copy.view_area_for_level)
{

}

//Destructors =========================
Champion::~Champion()
{

}

//Functionality =======================
//Actions -------------------
void Champion::Hability_A(...)
{

}

void Champion::Hability_B(...)
{

}

//Set Methods ---------------
void Champion::SetLevel(uint lvl)
{
	level = lvl;
}

void Champion::SetAttackForLevel(uint atk_for_lvl)
{
	attack_for_level = atk_for_lvl;
}

void Champion::SetRangeForLevel(uint rng_for_lvl)
{
	range_for_level = rng_for_lvl;
}

void Champion::SetDefenseForLevel(float def_for_lvl)
{
	defense_for_level = def_for_lvl;
}

void Champion::SetArmorForLevel(float arm_for_lvl)
{
	armor_for_level = arm_for_lvl;
}

void Champion::SetSpeedForLevel(float spd_for_lvl)
{
	speed_for_level = spd_for_lvl;
}

void Champion::SetViewAreaForLevel(uint view_for_level)
{
	view_area_for_level = view_for_level;
}

//Get Methods -------------------------
uint Champion::GetLevel() const
{
	return level;
}

uint Champion::GetAttackForLevel() const
{
	return attack_for_level;
}

uint Champion::GetRangeForLevel() const
{
	return range_for_level;
}

float Champion::GetDefenseForLevel() const
{
	return defense_for_level;
}

float Champion::GetArmorForLevel() const
{
	return armor_for_level;
}

float Champion::GetSpeedForLevel() const
{
	return speed_for_level;
}

uint Champion::GetViewAreaForLevel() const
{
	return view_area_for_level;
}
/// ---------------------------------------------

/// Class Warrior -------------------------------
// Class that defines the warrior champion states
//Constructors ========================
Warrior::Warrior() :Champion()
{

}

Warrior::Warrior(const Warrior & copy): Champion(copy),special_attack_area(copy.special_attack_area)
{

}

//Destructors =========================
Warrior::~Warrior()
{

}

bool Warrior::Draw(bool debug)
{
	bool ret = false;

	//Draw Entity Mark
	mark.Draw();

	//Draw warrior special attack area
	int x, y;
	App->input->GetMousePosition(x, y);
	CalculateSpecialAttackArea(iPoint(x - App->render->camera.x, y - App->render->camera.y));
	special_attack_area.Draw();

	//Draw Entity Current animation frame
	const Sprite* sprite = current_animation->GetCurrentSprite();
	ret = App->render->CallBlit(current_animation->GetTexture(), position.x, position.y, sprite->GetFrame(), flip_sprite, -position.y - sprite->GetZ_cord(), sprite->GetOpacity(), sprite->GetXpivot(), sprite->GetYpivot());

	return ret;
}

//Functionality =======================
//Actions -------------------
void Warrior::Hability_A(...)
{

}

void Warrior::CalculateSpecialAttackArea(const iPoint & base)
{
	//Update the triangle base
	special_attack_area.SetBase(base);
	//Recalculate the triangle vertex with the new base
	special_attack_area.CalculateVertex();
}

//Set Methods ---------------
void Warrior::SetPosition(float x, float y)
{
	//Set Warrior position 
	position.x = x;
	position.y = y;
	//Set mark position
	mark.SetPosition(iPoint(x, y));
	//Set attack area position
	special_attack_area.SetPosition(iPoint(x, y));
}
void Warrior::SetSpecialAttackArea(const Triangle & tri)
{
	special_attack_area = tri;
}


/// ---------------------------------------------



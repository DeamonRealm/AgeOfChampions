#include "Champions.h"

#include "j1App.h"
#include "j1Animator.h"
#include "j1Render.h"
#include "j1EntitiesManager.h"
#include "j1Input.h"
#include "j1BuffManager.h"

///Class Champion -------------------------------
//Base class that define the champions bases

//Constructors ========================
Champion::Champion() : Unit()
{

}

Champion::Champion(const Champion & copy) : Unit(copy), buff_area(copy.buff_area),buff_to_apply(copy.buff_to_apply), level(copy.level), attack_for_level(copy.attack_for_level), range_for_level(copy.range_for_level),
defense_for_level(copy.defense_for_level), armor_for_level(copy.armor_for_level), speed_for_level(copy.speed_for_level), view_area_for_level(copy.view_area_for_level)
{

}

//Destructors =========================
Champion::~Champion()
{

}

//Functionality =======================
//Actions -------------------
void Champion::Hability_A()
{

}

void Champion::Hability_B()
{

}

//Set Methods ---------------
void Champion::SetPosition(float x, float y)
{
	//Extract the units to push it with the new position later
	App->entities_manager->units_quadtree.Exteract(&position);

	//Set unit position
	position.x = x;
	position.y = y;
	iPoint pos(position.x, position.y);
	//Set unit vision position
	vision.SetPosition(pos);
	//Set unit mark position
	mark.SetPosition(pos);
	//Set soft_collider mark position
	soft_collider.SetPosition(pos);
	//Set hard_collider mark position
	hard_collider.SetPosition(pos);
	//Set buff_area position
	buff_area.SetPosition(pos);
	//Set unit attack area position
	attack_area.SetPosition(pos);

	//Add the unit with the correct position in the correct quad tree
	App->entities_manager->units_quadtree.Insert(this, &position);
}
void Champion::SetBuffArea(const Circle & area)
{
	buff_area = area;
}

void Champion::SetBuffToApply(const PassiveBuff * buff)
{
	buff_to_apply = (PassiveBuff*)buff;
}

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
Circle Champion::GetBuffArea() const
{
	return buff_area;
}

PassiveBuff * Champion::GetBuffToApply() const
{
	return buff_to_apply;
}

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

	//Draw Warrior Selection Mark
	if(selected)mark.Draw();

	//Draw Warrior logic areas
	if (debug && selected) {
		if (selected)
		{
			attack_area.Draw();
			soft_collider.Draw();
			hard_collider.Draw();
			vision.Draw();
			buff_area.Draw();
		}
	}

	//Calculate  & draw warrior special attack area
	int x, y;
	App->input->GetMousePosition(x, y);
	CalculateSpecialAttackArea(iPoint(x - App->render->camera.x, y - App->render->camera.y));
	if(debug && selected) special_attack_area.Draw();

	//Draw Entity Current animation frame
	const Sprite* sprite = current_animation->GetCurrentSprite();
	ret = App->render->CallBlit(current_animation->GetTexture(), position.x, position.y, sprite->GetFrame(), flip_sprite, -position.y - sprite->GetZ_cord(), sprite->GetOpacity(), sprite->GetXpivot(), sprite->GetYpivot());

	return ret;
}

//Functionality =======================
//Actions -------------------
void Warrior::Hability_A()
{

	//Collect all the units in the buff area
	std::vector<Unit*> units_around;
	App->entities_manager->units_quadtree.CollectCandidates(units_around, buff_area);

	//Deactivate all the buffs)
	std::list<Unit*>::const_iterator unit = buffed_units.begin();
	while (unit != buffed_units.end())
	{
		App->buff_manager->RemoveTargetBuff(unit._Ptr->_Myval, buff_to_apply);
		unit++;
	}

	//Update units buffs (check for new units and old ones)
	uint size = units_around.size();
	for (uint k = 0; k < size; k++)
	{
		if (units_around[k]->GetPosition() != position)
		{
			App->buff_manager->CallBuff(units_around[k], PASSIVE_BUFF, ATTACK_BUFF);
			buffed_units.push_back(units_around[k]);
		}
	}
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
	//Extract the units to push it with the new position later
	App->entities_manager->units_quadtree.Exteract(&position);

	//Set unit position
	position.x = x;
	position.y = y;
	iPoint pos(position.x, position.y);
	//Set unit vision position
	vision.SetPosition(pos);
	//Set unit mark position
	mark.SetPosition(pos);
	//Set soft_collider mark position
	soft_collider.SetPosition(pos);
	//Set hard_collider mark position
	hard_collider.SetPosition(pos);
	//Set buff_area position
	buff_area.SetPosition(pos);
	//Set unit attack area position
	attack_area.SetPosition(pos);
	//Set attack area position
	special_attack_area.SetPosition(pos);

	//Add the unit with the correct position in the correct quad tree
	App->entities_manager->units_quadtree.Insert(this, &position);
}

void Warrior::SetSpecialAttackArea(const Triangle & tri)
{
	special_attack_area = tri;
}


/// ---------------------------------------------



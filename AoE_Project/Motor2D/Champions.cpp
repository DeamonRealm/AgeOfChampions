#include "Champions.h"

#include "j1App.h"
#include "j1Animator.h"
#include "j1Render.h"
#include "j1EntitiesManager.h"
#include "j1Input.h"
#include "j1ActionManager.h"
#include "j1Player.h"
#include "j1SoundManager.h"
#include "Hud_ActionPanel.h"
#include "Hud_GamePanel.h"
#include "j1FogOfWar.h"

///Class Champion -------------------------------
//Base class that define the champions bases

//Constructors ========================
Champion::Champion() : Unit()
{

}

Champion::Champion(const Champion & copy) : Unit(copy), buff_area(copy.buff_area), buff_to_apply(copy.buff_to_apply), level(copy.level), attack_for_level(copy.attack_for_level), range_for_level(copy.range_for_level),
defense_for_level(copy.defense_for_level), speed_for_level(copy.speed_for_level), view_area_for_level(copy.view_area_for_level), ability_lvl_2_cooldown(copy.ability_lvl_2_cooldown), ability_lvl_3_cooldown(copy.ability_lvl_3_cooldown),
level_up_particle(copy.level_up_particle), experience_lvl_2(copy.experience_lvl_2), experience_lvl_3(copy.experience_lvl_3)
{

}

//Destructors =========================
Champion::~Champion()
{

}

bool Champion::Die()
{
	
	if (action_type != DIE && action_type != DISAPPEAR)
	{
		if (GetDiplomacy() == DIPLOMACY::ALLY)
			App->entities_manager->GetExperienceFromUnit(unit_experience, DIPLOMACY::ALLY);
		else
			App->entities_manager->GetExperienceFromUnit(unit_experience, DIPLOMACY::ENEMY);

		App->entities_manager->ExtractChampion(this, this->GetDiplomacy());
		App->player->action_panel->HeroIsDead(this->unit_type);
		App->buff_manager->RemoveTargetBuffs(this);
		action_type = DIE;
		if (this->GetDiplomacy() == ALLY) App->player->game_panel->IncressPopulation(-1, false);
		App->entities_manager->AddDeathUnit(this);
		App->animator->UnitPlay(this);
		App->fog_of_war->ReleaseEntityFog(this);

	}
	else if (current_animation->IsEnd())
	{
		if (action_type == DIE)
		{
			action_type = DISAPPEAR;
			App->animator->UnitPlay(this);
		}
		else
		{
			//	App->entities_manager->RemoveDeathUnit(this);
			App->entities_manager->DeleteEntity(this);
			return true;
		}
	}
	return false;
}

bool Champion::Update()
{
	//Check if champion lvl up;
	if (to_level_up) LevelUp();
	if (level_up_animation) LevelUpAnimation();
	if (actived[0] && level >= 0)CheckHability_lvl_1();
	if (ability_lvl_2_prepare_mode)PrepareAbility_lvl_2();
	else if (actived[1] && level >= 1)CheckHability_lvl_2();
	if (ability_lvl_3_prepare_mode)PrepareAbility_lvl_3();
	else if (actived[2] && level >= 2)CheckHability_lvl_3();

	CheckCooldown();

	action_worker.Update();

	return true;
}

//Functionality =======================
//Actions -------------------
void Champion::CleanBuffedUnits()
{
	std::list<Unit*>::iterator unit = buffed_units.begin();

	while (unit != buffed_units.end())
	{
		if (unit._Ptr->_Myval != this)App->buff_manager->RemoveTargetBuff(unit._Ptr->_Myval, buff_to_apply);

		unit++;
	}
}

void Champion::CheckCooldown()
{
	if (ability_lvl_2_current_time < ability_lvl_2_cooldown)
		ability_lvl_2_current_time = ability_lvl_2_timer.Read();
	else
		ability_lvl_2_ready = true;
	if (ability_lvl_3_current_time < ability_lvl_3_cooldown)
		ability_lvl_3_current_time = ability_lvl_3_timer.Read();
	else
		ability_lvl_3_ready = true;
}
//Ability A methods
void Champion::SetAbility_lvl_1(bool choosed)
{
	ability[0] = choosed;
	actived[0] = true;
}

void Champion::Hability_lvl_1()
{

}

void Champion::CheckHability_lvl_1()
{

}

//Ability B methods
void Champion::SetAbility_lvl_2(bool choosed)
{
	ability[1] = choosed;
}

bool Champion::PrepareAbility_lvl_2()
{
	return true;

}

void Champion::Hability_lvl_2(int x, int y)
{

}

void Champion::CheckHability_lvl_2()
{

}

void Champion::Cancel_Ability_lvl_2()
{
	ability_lvl_2_prepare_mode = false;
}

void Champion::SetAbility_lvl_3(bool choosed)
{
}

bool Champion::PrepareAbility_lvl_3()
{
	return true;
}

void Champion::Hability_lvl_3(int x, int y)
{
}

void Champion::CheckHability_lvl_3()
{
}

void Champion::Cancel_Ability_lvl_3()
{
	ability_lvl_3_prepare_mode = false;
}

void Champion::LevelUp(bool particle)
{
	if (level >= MAX_LEVEL) return;

	if (particle)SetLevelUpParticle();
	App->buff_manager->RemoveTargetBuffs(this);
	to_level_up = false;
	level_up_particle.position = this->GetPositionRounded();
	level_up_particle.animation.Reset();
	//Upgrade level
	level++;
	//Upgrade attack
	attack_hitpoints += attack_for_level;
	//Upgrade armor
	defense += defense_for_level;
	//Upgrade life
	max_life += life_for_level;
//	life += life_for_level;
	level_up_animation = particle;
}

void Champion::LevelUpAnimation()
{
	if (!level_up_particle.animation.IsEnd())
	{
		level_up_particle.Draw();
	}
	else
	{
		level_up_animation = false;
	}
}

void Champion::SetExperience(int exp)
{
	if (level >= MAX_LEVEL) return;

	current_experience += exp;
	if (level < 1)
	{
		if (current_experience > experience_lvl_2)
		{
			to_level_up = true;

		}
	}
	else
	{
		if (current_experience > experience_lvl_3)
		{
			to_level_up = true;
		}
	}
}

void Champion::SetSkillChoosed(bool val, uint index)
{
	skill_choosed[index] = val;
}

void Champion::SetLevelUpParticle()
{
	level_up_particle = App->buff_manager->GetParticle(LEVEL_UP_PARTICLE, NO_DIRECTION);
	App->sound->PlayFXAudio(LEVEL_UP_SOUND);
}

//Set Methods ---------------
void Champion::SetPosition(float x, float y, bool insert)
{
	//Extract the units to push it with the new position later
	App->entities_manager->units_quadtree.Exteract(this, &position);

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
	if (insert)App->entities_manager->units_quadtree.Insert(this, &position);
}
void Champion::SetBuffArea(const Circle & area)
{
	buff_area = area;
}

void Champion::SetBuffToApply(const PassiveBuff * buff)
{
	buff_to_apply = (PassiveBuff*)buff;
}

void Champion::SetLvl2Cap(uint value)
{
	experience_lvl_2 = value;
}

void Champion::SetLvl3Cap(uint value)
{
	experience_lvl_3 = value;
}

void Champion::SetAbility_lvl_2_Cooldown(uint value)
{
	ability_lvl_2_cooldown = value;
}

void Champion::SetAbility_lvl_3_Cooldown(uint value)
{
	ability_lvl_3_cooldown = value;
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

float Champion::GetSpeedForLevel() const
{
	return speed_for_level;
}

uint Champion::GetViewAreaForLevel() const
{
	return view_area_for_level;
}

bool Champion::GetAbilityChosen(uint index) const
{
	return ability[index];
}

bool Champion::GetSkillChoosed(uint index) const
{
	return skill_choosed[index];
}
/// ---------------------------------------------

/// Class Warrior -------------------------------
// Class that defines the warrior champion states
//Constructors ========================
Warrior::Warrior() :Champion()
{

}

Warrior::Warrior(const Warrior & copy) : Champion(copy), special_attack_area(copy.special_attack_area), ability_lvl_2_attack_value(copy.ability_lvl_2_attack_value), ability_lvl_2_stun_value(copy.ability_lvl_2_stun_value), area_ability_lvl_3(copy.area_ability_lvl_3)
{
	buff_to_apply = App->buff_manager->GetPassiveBuff(PASSIVE_BUFF, ATTACK_BUFF, false);
	ability_lvl_2_particle = App->buff_manager->GetParticle(SLASH_PARTICLE, SOUTH);
	ability_lvl_3_particle = App->buff_manager->GetParticle(TAUNT_PARTICLE, NO_DIRECTION);
	for (uint k = 0; k < 3; k++)
	{
		actived[k] = false;
		ability[k] = false;
		skill_choosed[k] = false;

	}
}

//Destructors =========================
Warrior::~Warrior()
{

}

void Warrior::ClearProtectedUnits()
{
	int size = protected_units.size();
	for (int i = 0; i < size; i++)
	{
		protected_units[i]->QuitProtection();
	}
}

void Warrior::SaveAsDef(pugi::xml_node & node)
{
	//Add all warrior attributes at the xml node

	//Name
	node.append_attribute("name") = "Warrior";

	/*Type*/	node.append_attribute("unit_type") = App->animator->EnumToUnitStr(unit_type);
	/*Class*/	node.append_attribute("unit_class") = App->animator->EnumToUnitClassStr(unit_class);
	/*Atk Type*/node.append_attribute("attack_type") = App->animator->EnumToAttackStr(attack_type);

	/*Selec X*/ node.append_attribute("selection_x") = selection_rect.x;
	/*Selec Y*/ node.append_attribute("selection_y") = selection_rect.y;
	/*Selec W*/ node.append_attribute("selection_w") = selection_rect.w;
	/*Selec H*/ node.append_attribute("selection_h") = selection_rect.h;

	/*Icon X*/	node.append_attribute("icon_x") = icon_rect.x;
	/*Icon Y*/	node.append_attribute("icon_y") = icon_rect.y;
	/*Icon W*/	node.append_attribute("icon_w") = icon_rect.w;
	/*Icon H*/	node.append_attribute("icon_h") = icon_rect.h;

	
	/*buff rad*/	node.append_attribute("buff_area_rad") = buff_area.GetRad();
	/*Vision rad*/	node.append_attribute("vision_rad") = vision.GetRad();
	/*Mark Rad*/	node.append_attribute("mark_rad") = mark.GetRad();
	/*Soft Rad*/	node.append_attribute("soft_rad") = soft_collider.GetRad();
	/*Hard Rad*/	node.append_attribute("hard_rad") = hard_collider.GetRad();



	/*Max Life*/	node.append_attribute("max_life") = max_life;
	/*Life Pos X*/	node.append_attribute("life_pos_x") = life_position.x;
	/*Life Pos Y*/	node.append_attribute("life_pos_y") = life_position.y;
	/*Speed*/		node.append_attribute("speed") = speed;
	/*Atk HitP*/	node.append_attribute("attack_hitpoints") = attack_hitpoints;
	/*Atk Rate*/	node.append_attribute("attack_rate") = attack_rate;
	/*Atk Range*/	node.append_attribute("attack_range") = attack_area.GetRad();
	/*Defense*/		node.append_attribute("defense") = defense;
	/*Food Cost*/	node.append_attribute("food_cost") = food_cost;
	/*Wood Cost*/	node.append_attribute("wood_cost") = wood_cost;
	/*Gold Cost*/	node.append_attribute("gold_cost") = gold_cost;
	/*Popu Cost*/	node.append_attribute("population_cost") = population_cost;
	/*Train Time*/	node.append_attribute("train_time") = train_time;
	/*Experience*/	node.append_attribute("unit_experience") = unit_experience;

	/*Exp lvl 2*/			node.append_attribute("experience_lvl_2") = experience_lvl_2;
	/*Exp lvl 3*/			node.append_attribute("experience_lvl_3") = experience_lvl_3;
	/*Ab lvl 2 coold*/		node.append_attribute("ability_lvl_2_cooldown") = ability_lvl_2_cooldown;
	/*Ab lvl 2 atk val*/	node.append_attribute("ability_lvl_2_attack_value") = ability_lvl_2_attack_value;
	/*Ab lvl 2 stun val*/	node.append_attribute("ability_lvl_2_stun_value") = ability_lvl_2_stun_value;
	/*Ab lvl 3 coold*/		node.append_attribute("ability_lvl_3_cooldown") = ability_lvl_3_cooldown;
	/*Area ab lvl 3*/		node.append_attribute("area_ability_lvl_3") = area_ability_lvl_3.GetRad();

	/*Atk for lvl*/			node.append_attribute("attack_for_level") = attack_for_level;
	/*Range for lvl*/		node.append_attribute("range_for_level") = range_for_level;
	/*Life for lvl*/		node.append_attribute("life_for_level") = life_for_level;
	/*Def for lvl*/			node.append_attribute("defense_for_level") = defense_for_level;
	/*Speed for lvl*/		node.append_attribute("speed_for_level") = speed_for_level;
	/*View for lvl*/		node.append_attribute("view_area_for_level") = view_area_for_level;
	/*Atk Tri Lenght*/		node.append_attribute("atk_triangle_length") = this->special_attack_area.GetLength();
	/*Atk Tri Width*/		node.append_attribute("atk_triangle_width_angle") = this->special_attack_area.GetWidthAngle();

}

bool Warrior::Draw(bool debug)
{
	bool ret = false;

	//Draw Warrior Selection Mark
	if (selected)
	{
		if (ability_lvl_2_prepare_mode)special_attack_area.Draw();
		mark.Draw();
	}

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

	//Calculate & draw warrior special attack area
	int x, y;
	App->input->GetMousePosition(x, y);
	CalculateSpecialAttackArea(iPoint(x - App->render->camera.x, y - App->render->camera.y));
	if (debug && selected) special_attack_area.Draw();

	//Draw Entity Current animation frame
	const Sprite* sprite = current_animation->GetCurrentSprite();
	ret = App->render->CallBlit(current_animation->GetTexture(), position.x, position.y, sprite->GetFrame(), flip_sprite, -position.y - sprite->GetZ_cord(), sprite->GetOpacity(), sprite->GetXpivot(), sprite->GetYpivot(), &blit_color);

	return ret;
}



//Functionality =======================
//Actions -------------------
//Ability A methods
void Warrior::SetAbility_lvl_1(bool choosed)
{
	if (actived[0])return;

	if (choosed)
	{
		buff_to_apply = App->buff_manager->GetPassiveBuff(PASSIVE_BUFF, ATTACK_BUFF, false);
	}
	else
	{
		buff_to_apply = App->buff_manager->GetPassiveBuff(PASSIVE_BUFF, DEFENSE_BUFF, false);
	}
	skill_choosed[0] = true;
	ability[0] = choosed;

}

void Warrior::Hability_lvl_1()
{
	if (actived[0])return;
	//Collect all the units in the buff area
	std::vector<Unit*> units_around;
	App->entities_manager->units_quadtree.CollectCandidates(units_around, buff_area);
	uint size = units_around.size();

	//Update units buffs (check for new units and old ones)
	for (uint k = 0; k < size; k++)
	{
		if (units_around[k]->GetDiplomacy() == entity_diplomacy)
		{
			if (units_around[k]->GetPosition() != position)
			{
				App->buff_manager->CallBuff(units_around[k], buff_to_apply->GetBuffType(), buff_to_apply->GetAttributeType(), false);
				buffed_units.push_back(units_around[k]);
			}
			else
			{
				App->buff_manager->CallBuff(units_around[k], buff_to_apply->GetBuffType(), buff_to_apply->GetAttributeType(), true);
				buffed_units.push_back(units_around[k]);
			}
		}
	}
	actived[0] = true;
}

void Warrior::CheckHability_lvl_1()
{
	//Collect all the units in the buff area
	std::vector<Unit*> units_around;
	App->entities_manager->units_quadtree.CollectCandidates(units_around, buff_area);
	uint size = units_around.size();

	//Deactivate all the units buffs that are not in the buff area
	std::list<Unit*>::iterator unit = buffed_units.begin();
	while (unit != buffed_units.end())
	{
		bool found = false;
		for (uint k = 0; k < size; k++)
		{
			if (unit._Ptr->_Myval == units_around[k])
			{
				if (App->buff_manager->CheckBuff(units_around[k], buff_to_apply->GetBuffType(), buff_to_apply->GetAttributeType()))
					units_around[k] = nullptr;
				found = true;
				break;
			}
		}

		if (!found)
		{
			App->buff_manager->RemoveTargetBuff(unit._Ptr->_Myval, buff_to_apply);
			buffed_units.remove(unit._Ptr->_Myval);
		}
		unit++;
	}

	//Update units buffs (check for new units and old ones)
	for (uint k = 0; k < size; k++)
	{
		if (units_around[k] == nullptr)continue;
		if (units_around[k]->GetDiplomacy() == entity_diplomacy && units_around[k]->GetPosition() != position)
		{

			App->buff_manager->CallBuff(units_around[k], buff_to_apply->GetBuffType(), buff_to_apply->GetAttributeType(), false);
			buffed_units.push_back(units_around[k]);
		}
	}
}

void Warrior::SetAbility_lvl_2(bool choosed)
{
	if (level < 1) return;

	if (choosed)
	{
		ability_lvl_2_particle = App->buff_manager->GetParticle(SLASH_PARTICLE, SOUTH);
		ability_lvl_2_current_time = ability_lvl_2_cooldown;
		ability_lvl_2_ready = true;

	}
	else
	{
		ability_lvl_2_particle = App->buff_manager->GetParticle(STUN_PARTICLE, SOUTH);
		ability_lvl_2_current_time = ability_lvl_2_cooldown;
		ability_lvl_2_ready = true;
	}
	skill_choosed[1] = true;
	ability[1] = choosed;
}

bool Warrior::PrepareAbility_lvl_2()
{
	if (!ability_lvl_2_ready)return false;

	ability_lvl_2_prepare_mode = true;

	//Focus the warrior in the attack direction
	int x, y;
	App->input->GetMousePosition(x, y);
	direction_type = LookDirection(iPoint(x - App->render->camera.x, y - App->render->camera.y), GetPositionRounded());
	CalculateSpecialAttackArea(GetiPointFromDirection(direction_type));

	//Collect all the units in the buff area
	std::vector<Unit*> units_in;
	uint size = App->entities_manager->units_quadtree.CollectCandidates(units_in, special_attack_area);

	for (uint k = 0; k < size; k++)
	{
		if (units_in[k]->GetDiplomacy() == entity_diplomacy)continue;
		units_in[k]->SetBlitColor({ 255,150,150,255 });
	}
	return true;
}

void Warrior::Hability_lvl_2(int x, int y)
{
	ability_lvl_2_prepare_mode = false;

	if (ability_lvl_2_current_time < ability_lvl_2_cooldown)return;

	//Focus the warrior in the attack direction

	direction_type = LookDirection(iPoint(x, y), GetPositionRounded());
	CalculateSpecialAttackArea(GetiPointFromDirection(direction_type));
	action_type = ATTATCK;
	App->animator->UnitPlay(this);


	if (ability[1])
	{
		ability_lvl_2_particle = App->buff_manager->GetParticle(SLASH_PARTICLE, direction_type);
		App->sound->PlayFXAudio(SOUND_TYPE::WARRIOR_SKILL_LVL2_B);
	}
	else
	{
		ability_lvl_2_particle = App->buff_manager->GetParticle(STUN_PARTICLE, direction_type);
		App->sound->PlayFXAudio(SOUND_TYPE::WARRIOR_SKILL_LVL2_A);
	}
	//Reset Particles
	ability_lvl_2_particle.animation.Reset();
	ability_lvl_2_particle.position = GetPositionRounded();

	//Collect all the units in the buff area
	std::vector<Unit*> units_in;
	App->entities_manager->units_quadtree.CollectCandidates(units_in, special_attack_area);

	//Apply effects
	uint size = units_in.size();
	for (uint k = 0; k < size; k++)
	{
		if (units_in[k]->GetDiplomacy() == entity_diplomacy)continue;

		if (units_in[k]->GetPosition() != position)
		{
			if (ability[1])
			{
				units_in[k]->DirectDamage(ability_lvl_2_attack_value);
			}
			else
			{
				App->buff_manager->CallBuff(units_in[k], TIMED_BUFF, STUN_BUFF);
				units_in[k]->Stun(ability_lvl_2_stun_value);

			}
		}
	}

	actived[1] = true;

	ability_lvl_2_timer.Start();
	ability_lvl_2_current_time = 0;
	ability_lvl_2_ready = false;

}

void Warrior::CheckHability_lvl_2()
{
	if (!ability_lvl_2_particle.animation.IsEnd())
	{
		ability_lvl_2_particle.Draw();
	}
	else
	{
		actived[1] = false;
		action_type = IDLE;
		App->animator->UnitPlay(this);
	}
}

void Warrior::SetAbility_lvl_3(bool choosed)
{
	if (level < 2) return;

	if (choosed)
	{
		ability_lvl_3_particle = App->buff_manager->GetParticle(ONE_HIT_PARTICLE, NO_DIRECTION);
		ability_lvl_3_current_time = ability_lvl_3_cooldown;
		ability_lvl_3_ready = true;

	}
	else
	{
		ability_lvl_3_particle = App->buff_manager->GetParticle(TAUNT_PARTICLE, NO_DIRECTION);
		ability_lvl_3_current_time = ability_lvl_3_cooldown;
		ability_lvl_3_ready = true;

	}
	skill_choosed[2] = true;
	ability[2] = choosed;
}

bool Warrior::PrepareAbility_lvl_3()
{
	if (!ability_lvl_3_ready)return false;

	ability_lvl_3_prepare_mode = true;
	return true;
}

void Warrior::Hability_lvl_3(int x, int y)
{
	ability_lvl_3_prepare_mode = false;
	if (ability_lvl_3_current_time < ability_lvl_3_cooldown)return;

	if (ability[2])
	{

		App->sound->PlayFXAudio(SOUND_TYPE::WARRIOR_SKILL_LVL2_A);//lvl_3_defense
		App->buff_manager->CallBuff(this, TIMED_BUFF, SUPER_ATTACK_BUFF, true);
	}
	else
	{
		App->sound->PlayFXAudio(SOUND_TYPE::WARRIOR_SKILL_LVL2_B);//lvl_3_attack
																  //Collect all the units in the buff area
		std::vector<Unit*> units_in;
		App->entities_manager->units_quadtree.CollectCandidates(units_in, area_ability_lvl_3);
		uint size = units_in.size();
		for (uint k = 0; k < size; k++)
		{
			if (units_in[k]->GetDiplomacy() != entity_diplomacy)continue;
			if (units_in[k] != this)
			{
				App->buff_manager->CallBuff(units_in[k], TIMED_BUFF, TAUNT_BUFF);
				units_in[k]->SetProtection(this);
				protected_units.push_back(units_in[k]);
			}
		}
		
	}
	//Particle Reset
	ability_lvl_3_particle.animation.Reset();
	ability_lvl_3_particle.position = GetPositionRounded();
	actived[2] = true;
	ability_lvl_3_timer.Start();
	ability_lvl_3_current_time = 0;
	ability_lvl_3_ready = false;

}

void Warrior::CheckHability_lvl_3()
{

	if (!ability_lvl_3_particle.animation.IsEnd())
	{
		ability_lvl_3_particle.position = GetPositionRounded();
		ability_lvl_3_particle.Draw();
	}
	else
	{
		actived[2] = false;
		action_type = IDLE;
		App->animator->UnitPlay(this);
	}
}

iPoint Warrior::GetiPointFromDirection(DIRECTION_TYPE direction) const
{

	switch (direction)
	{
	case NORTH:
		return iPoint(position.x, position.y - 1);
		break;
	case NORTH_EAST:
		return iPoint(position.x + 1, position.y - 1);
		break;
	case EAST:
		return iPoint(position.x + 1, position.y);
		break;
	case SOUTH_EAST:
		return iPoint(position.x + 1, position.y + 1);
		break;
	case SOUTH:
		return iPoint(position.x, position.y + 1);
		break;
	case SOUTH_WEST:
		return iPoint(position.x - 1, position.y + 1);
		break;
	case WEST:
		return iPoint(position.x - 1, position.y);
		break;
	case NORTH_WEST:
		return iPoint(position.x - 1, position.y - 1);
		break;
	}
	return iPoint(position.x - 1, position.y - 1);;
}

void Warrior::CalculateSpecialAttackArea(const iPoint& base)
{
	//Update the triangle base
	special_attack_area.SetBase(base);
	//Recalculate the triangle vertex with the new base
	special_attack_area.CalculateVertex();
}


//Set Methods ---------------
void Warrior::SetPosition(float x, float y, bool insert)
{
	//Extract the units to push it with the new position later
	if (insert)App->entities_manager->units_quadtree.Exteract(this, &position);

	//Set unit position
	position.x = x;
	position.y = y;
	iPoint pos(position.x, position.y);
	//Set unit vision position
	vision.SetPosition(pos);
	//Set unit render area position
	render_area.SetPosition(pos);
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
	area_ability_lvl_3.SetPosition(pos);

	//Check unit fog
	SDL_Point point = { pos.x,pos.y };
	int dist_mult = 10;
	if (SDL_PointInRect(&point, &App->render->camera_viewport))dist_mult = 2;
	if (entity_diplomacy == ALLY && (this->distance_walked.x == 0.00f || abs(distance_walked.x) + abs(distance_walked.y) > App->fog_of_war->alpha_cell_size * dist_mult))App->fog_of_war->CheckEntityFog(this);

	//Add the unit with the correct position in the correct quad tree
	if (insert)App->entities_manager->units_quadtree.Insert(this, &position);
}

void Warrior::SetSpecialAttackArea(const Triangle & tri)
{
	special_attack_area = tri;
}

void Warrior::SetAreaAbilityLvl3(const Circle & circle)
{
	area_ability_lvl_3 = circle;

}

void Warrior::SetAbility_lvl_2_AttackValue(uint atk)
{
	ability_lvl_2_attack_value = atk;
}

void Warrior::SetAbility_lvl_2_StunValue(uint stun)
{
	ability_lvl_2_stun_value = stun;
}




/// ---------------------------------------------
Wizard::Wizard() :Champion()
{

}

Wizard::Wizard(const Wizard & copy) : Champion(copy), area_attack_spell_2(copy.area_attack_spell_2), area_limit_spell_2(copy.area_limit_spell_2), area_attack_spell_3(copy.area_attack_spell_3), area_limit_spell_3(copy.area_limit_spell_3), ability_lvl_2_heal_value(copy.ability_lvl_2_heal_value), ability_lvl_3_attack_value(copy.ability_lvl_3_attack_value)
{
	buff_to_apply = App->buff_manager->GetPassiveBuff(PASSIVE_BUFF, VISION_BUFF, false);
	ability_lvl_2_A_particle = App->buff_manager->GetParticle(HEAL_PARTICLE, NO_DIRECTION);
	ability_lvl_2_B_particle = App->buff_manager->GetParticle(TELEPORT_PARTICLE, NO_DIRECTION);
	radius_hit_lvl_2_particle = App->buff_manager->GetParticle(RADIUS_HIT_BIG_PARTICLE, NO_DIRECTION);
	radius_hit_lvl_2_particle_teleport = App->buff_manager->GetParticle(RADIUS_HIT_BIG_PARTICLE, NO_DIRECTION);
	radius_hit_lvl_3_particle = App->buff_manager->GetParticle(RADIUS_HIT_BIG_PARTICLE, NO_DIRECTION);

	for (uint k = 0; k < 3; k++)
	{
		actived[k] = false;
		ability[k] = false;
		skill_choosed[k] = false;

	}
}

//Destructors =========================
Wizard::~Wizard()
{

}
void Wizard::SaveAsDef(pugi::xml_node & node)
{
	//Add all wizard attributes at the xml node

	//Name
	node.append_attribute("name") = "Wizard";

	/*Type*/	node.append_attribute("unit_type") = App->animator->EnumToUnitStr(unit_type);
	/*Class*/	node.append_attribute("unit_class") = App->animator->EnumToUnitClassStr(unit_class);
	/*Atk Type*/node.append_attribute("attack_type") = App->animator->EnumToAttackStr(attack_type);

	/*Selec X*/ node.append_attribute("selection_x") = selection_rect.x;
	/*Selec Y*/ node.append_attribute("selection_y") = selection_rect.y;
	/*Selec W*/ node.append_attribute("selection_w") = selection_rect.w;
	/*Selec H*/ node.append_attribute("selection_h") = selection_rect.h;

	/*Icon X*/	node.append_attribute("icon_x") = icon_rect.x;
	/*Icon Y*/	node.append_attribute("icon_y") = icon_rect.y;
	/*Icon W*/	node.append_attribute("icon_w") = icon_rect.w;
	/*Icon H*/	node.append_attribute("icon_h") = icon_rect.h;

	/*buff rad*/	node.append_attribute("buff_area_rad") = buff_area.GetRad();
	/*Vision rad*/	node.append_attribute("vision_rad") = vision.GetRad();
	/*Mark Rad*/	node.append_attribute("mark_rad") = mark.GetRad();
	/*Soft Rad*/	node.append_attribute("soft_rad") = soft_collider.GetRad();
	/*Hard Rad*/	node.append_attribute("hard_rad") = hard_collider.GetRad();


	/*Max Life*/	node.append_attribute("max_life") = max_life;
	/*Life Pos X*/	node.append_attribute("life_pos_x") = life_position.x;
	/*Life Pos Y*/	node.append_attribute("life_pos_y") = life_position.y;
	/*Speed*/		node.append_attribute("speed") = speed;
	/*Atk HitP*/	node.append_attribute("attack_hitpoints") = attack_hitpoints;
	/*Atk Rate*/	node.append_attribute("attack_rate") = attack_rate;
	/*Atk Range*/	node.append_attribute("attack_range") = attack_area.GetRad();
	/*Defense*/		node.append_attribute("defense") = defense;
	/*Food Cost*/	node.append_attribute("food_cost") = food_cost;
	/*Wood Cost*/	node.append_attribute("wood_cost") = wood_cost;
	/*Gold Cost*/	node.append_attribute("gold_cost") = gold_cost;
	/*Popu Cost*/	node.append_attribute("population_cost") = population_cost;
	/*Train Time*/	node.append_attribute("train_time") = train_time;
	/*Experience*/	node.append_attribute("unit_experience") = unit_experience;

	/*Exp lvl 2*/			node.append_attribute("experience_lvl_2") = experience_lvl_2;
	/*Exp lvl 3*/			node.append_attribute("experience_lvl_3") = experience_lvl_3;
	
	/*Ab lvl 2 coold*/		node.append_attribute("ability_lvl_2_cooldown") = ability_lvl_2_cooldown;
	/*Ab lvl 2 atk val*/	node.append_attribute("ability_lvl_2_heal_value") = ability_lvl_2_heal_value;
	/*Ab lvl 2 stun val*/	node.append_attribute("ability_lvl_3_attack_value") = ability_lvl_3_attack_value;
	/*Ab lvl 3 coold*/		node.append_attribute("ability_lvl_3_cooldown") = ability_lvl_3_cooldown;

	/*Atk for lvl*/			node.append_attribute("attack_for_level") = attack_for_level;
	/*Range for lvl*/		node.append_attribute("range_for_level") = range_for_level;
	/*Life for lvl*/		node.append_attribute("life_for_level") = life_for_level;
	/*Def for lvl*/			node.append_attribute("defense_for_level") = defense_for_level;
	/*Speed for lvl*/		node.append_attribute("speed_for_level") = speed_for_level;
	/*View for lvl*/		node.append_attribute("view_area_for_level") = view_area_for_level;

	/*Area Atk spell 2*/	node.append_attribute("area_attack_spell_2") = area_attack_spell_2.GetRad();
	/*Area Limit spell 2*/	node.append_attribute("area_limit_spell_2") = area_limit_spell_2;
	/*Area Atk spell 3*/	node.append_attribute("area_attack_spell_3") = area_attack_spell_3.GetRad();
	/*Area Limit spell 3*/	node.append_attribute("area_limit_spell_3") = area_limit_spell_3;

}


bool Wizard::Draw(bool debug)
{
	bool ret = false;
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint mouse_position(x - App->render->camera.x, y - App->render->camera.y);

	//Draw wizard Selection Mark
	bool check = false;

	if (selected)
	{
		if (ability_lvl_2_prepare_mode)
		{
			check = CalculateSpecialAttackArea(mouse_position, true);
		
			if (ability[1])
			{	
				if (!check)
					radius_hit_lvl_2_particle_teleport.SetBlitColor({ 255,10,10,50 });
				radius_hit_lvl_2_particle_teleport.position = this->GetPositionRounded();
				radius_hit_lvl_2_particle_teleport.Draw();
			}
			if (!check)
				radius_hit_lvl_2_particle.SetBlitColor({ 255,10,10,50 });
			radius_hit_lvl_2_particle.position = mouse_position;
			radius_hit_lvl_2_particle.Draw();
			//area_attack_spell_2.Draw();

		}
		else if (ability_lvl_3_prepare_mode)
		{
			check = CalculateSpecialAttackArea(mouse_position, false);
			if (!check)
				radius_hit_lvl_3_particle.SetBlitColor({ 255,10,10,255 });
			radius_hit_lvl_3_particle.position = mouse_position;
			radius_hit_lvl_3_particle.Draw();
			//area_attack_spell_3.Draw();
		}
		mark.Draw();
	}
	//Draw wizard logic areas
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

	//Calculate & draw wizard special attack area

	CalculateSpecialAttackArea(mouse_position, true);
	if (debug && selected) {
		//area_limit_spell_2.Draw();
		//area_limit_spell_3.Draw();

	}
	//Draw Entity Current animation frame
	const Sprite* sprite = current_animation->GetCurrentSprite();
	ret = App->render->CallBlit(current_animation->GetTexture(), position.x, position.y, sprite->GetFrame(), flip_sprite, -position.y - sprite->GetZ_cord(), sprite->GetOpacity(), sprite->GetXpivot(), sprite->GetYpivot(), &blit_color);

	return ret;
}

void Wizard::SetAbility_lvl_1(bool choosed)
{
	if (actived[0])return;

	if (choosed)
	{
		buff_to_apply = App->buff_manager->GetPassiveBuff(PASSIVE_BUFF, LIFE_BUFF, false);
	}
	else
	{
		buff_to_apply = App->buff_manager->GetPassiveBuff(PASSIVE_BUFF, VISION_BUFF, false);
	}
	skill_choosed[0] = true;
	ability[0] = choosed;

}

void Wizard::Hability_lvl_1()
{

	if (actived[0])return;
	//Collect all the units in the buff area
	std::vector<Unit*> units_around;
	App->entities_manager->units_quadtree.CollectCandidates(units_around, buff_area);
	uint size = units_around.size();

	//Update units buffs (check for new units and old ones)
	for (uint k = 0; k < size; k++)
	{
		if (units_around[k]->GetDiplomacy() == entity_diplomacy)
		{
			if (units_around[k]->GetPosition() != position)
			{
				App->buff_manager->CallBuff(units_around[k], buff_to_apply->GetBuffType(), buff_to_apply->GetAttributeType(), false);
				buffed_units.push_back(units_around[k]);
			}
			else
			{
				App->buff_manager->CallBuff(units_around[k], buff_to_apply->GetBuffType(), buff_to_apply->GetAttributeType(), true);
				buffed_units.push_back(units_around[k]);
			}
		}
	}
	actived[0] = true;
}

void Wizard::CheckHability_lvl_1()
{

	//Collect all the units in the buff area
	std::vector<Unit*> units_around;
	App->entities_manager->units_quadtree.CollectCandidates(units_around, buff_area);
	uint size = units_around.size();

	//Deactivate all the units buffs that are not in the buff area
	std::list<Unit*>::iterator unit = buffed_units.begin();
	while (unit != buffed_units.end())
	{
		bool found = false;
		for (uint k = 0; k < size; k++)
		{
			if (unit._Ptr->_Myval == units_around[k])
			{
				if (App->buff_manager->CheckBuff(units_around[k], buff_to_apply->GetBuffType(), buff_to_apply->GetAttributeType()))
					units_around[k] = nullptr;					
				found = true;
				break;
			}
		}

		if (!found)
		{
			App->buff_manager->RemoveTargetBuff(unit._Ptr->_Myval, buff_to_apply);
			buffed_units.remove(unit._Ptr->_Myval);
		}
		unit++;
	}

	//Update units buffs (check for new units and old ones)
	for (uint k = 0; k < size; k++)
	{
		if (units_around[k] == nullptr)continue;
		if (units_around[k]->GetDiplomacy() == entity_diplomacy && units_around[k]->GetPosition() != position)
		{
			App->buff_manager->CallBuff(units_around[k], buff_to_apply->GetBuffType(), buff_to_apply->GetAttributeType(), false);
			buffed_units.push_back(units_around[k]);
		}
	}
}

void Wizard::SetAbility_lvl_2(bool choosed)
{
	if (level < 1) return;

	if (choosed)
	{
		ability_lvl_2_A_particle = App->buff_manager->GetParticle(TELEPORT_PARTICLE, NO_DIRECTION);
		ability_lvl_2_B_particle = App->buff_manager->GetParticle(TELEPORT_PARTICLE, NO_DIRECTION);
		ability_lvl_2_current_time = ability_lvl_2_cooldown;
		ability_lvl_2_ready = true;
		radius_hit_lvl_2_particle_teleport = App->buff_manager->GetParticle(RADIUS_HIT_BIG_PARTICLE, NO_DIRECTION);
	}
	else
	{
		ability_lvl_2_A_particle = App->buff_manager->GetParticle(HEAL_PARTICLE, NO_DIRECTION);
		ability_lvl_2_current_time = ability_lvl_2_cooldown;
		ability_lvl_2_ready = true;

	}
	radius_hit_lvl_2_particle = App->buff_manager->GetParticle(RADIUS_HIT_BIG_PARTICLE, NO_DIRECTION);
	skill_choosed[1] = true;
	ability[1] = choosed;
}

bool Wizard::PrepareAbility_lvl_2()
{
	if (!ability_lvl_2_ready)return false;

	ability_lvl_2_prepare_mode = true;

	//Focus the wizard in the attack direction
	int x, y;
	App->input->GetMousePosition(x, y);
	direction_type = LookDirection(iPoint(x - App->render->camera.x, y - App->render->camera.y), GetPositionRounded());
	CalculateSpecialAttackArea(iPoint(x - App->render->camera.x, y - App->render->camera.y), true);

	//Collect all the units in the buff area
	std::vector<Unit*> units_in;
	uint size = 0;
	if (ability[1])
	{
		Circle temp_circle = area_attack_spell_2;
		temp_circle.SetPosition(this->GetPositionRounded());

		size = App->entities_manager->units_quadtree.CollectCandidates(units_in, temp_circle);

	}
	else
	{
		size = App->entities_manager->units_quadtree.CollectCandidates(units_in, area_attack_spell_2);

	}


	for (uint k = 0; k < size; k++)
	{
		if (units_in[k]->GetDiplomacy() != entity_diplomacy)continue;

		if (ability[1]) {
			units_in[k]->SetBlitColor({ 150,150,255,255 });
		}
		else {
			units_in[k]->SetBlitColor({ 150,255,150,255 });
		}
	}
	return true;
}

void Wizard::Hability_lvl_2(int x, int y)
{
	ability_lvl_2_prepare_mode = false;

	if (ability_lvl_2_current_time < ability_lvl_2_cooldown)return;

	//Focus the wizard in the spell direction
	iPoint destination = iPoint(x, y);
	direction_type = LookDirection(destination, GetPositionRounded());
	if (!CalculateSpecialAttackArea(destination, true))
		return;
	action_type = ATTATCK;
	App->animator->UnitPlay(this);
	iPoint teleport_movement = { 0,0 };

	std::vector<Unit*> units_in;

	if (ability[1])
	{
		iPoint wizard_position = this->GetPositionRounded();
		App->sound->PlayFXAudio(SOUND_TYPE::WIZARD_SKILL_LVL2_B);
		teleport_movement = destination - wizard_position;
		Circle temp_circle = area_attack_spell_2;
		temp_circle.SetPosition(wizard_position);
		//Get Candidates
		App->entities_manager->units_quadtree.CollectCandidates(units_in, temp_circle);
		//Reset Particles
		ability_lvl_2_A_particle.animation.Reset();
		ability_lvl_2_A_particle.position = this->GetPositionRounded();
		ability_lvl_2_B_particle.animation.Reset();
		ability_lvl_2_B_particle.position = destination;
	}
	else
	{

		App->sound->PlayFXAudio(SOUND_TYPE::WIZARD_SKILL_LVL2_A);
		//Get Candidates
		App->entities_manager->units_quadtree.CollectCandidates(units_in, area_attack_spell_2);
		//Reset Particles
		ability_lvl_2_A_particle.animation.Reset();
		ability_lvl_2_A_particle.position = destination;
	}
	//Apply effects
	uint size = units_in.size();
	for (uint k = 0; k < size; k++)
	{
		if (units_in[k]->GetDiplomacy() != entity_diplomacy)continue;
		if (ability[1])
		{
			units_in[k]->AddPriorizedAction((Action*)App->action_manager->TeleportAction(units_in[k], teleport_movement));
		}
		else
		{
			units_in[k]->Heal(ability_lvl_2_heal_value);
		}
	}


	actived[1] = true;
	ability_lvl_2_timer.Start();
	ability_lvl_2_current_time = 0;
	ability_lvl_2_ready = false;

}

void Wizard::CheckHability_lvl_2()
{
	if (ability[1])
	{
		if (!ability_lvl_2_A_particle.animation.IsEnd()) ability_lvl_2_A_particle.Draw();

		if (!ability_lvl_2_B_particle.animation.IsEnd()) ability_lvl_2_B_particle.Draw();

		if (ability_lvl_2_B_particle.animation.IsEnd() && ability_lvl_2_A_particle.animation.IsEnd())
		{
			actived[1] = false;
			action_type = IDLE;
			App->animator->UnitPlay(this);
		}
	}
	else
	{
		if (!ability_lvl_2_A_particle.animation.IsEnd()) ability_lvl_2_A_particle.Draw();
		else
		{
			actived[1] = false;
			action_type = IDLE;
			App->animator->UnitPlay(this);

		}
	}

}
void Wizard::SetAbility_lvl_3(bool choosed)
{
	if (level < 2) return;

	if (choosed)
	{
		ability_lvl_3_particle = App->buff_manager->GetParticle(THUNDER_PARTICLE, NO_DIRECTION);
		ability_lvl_3_current_time = ability_lvl_3_cooldown;
		ability_lvl_3_ready = true;

	}
	else
	{
		ability_lvl_3_particle = App->buff_manager->GetParticle(STUN_PARTICLE, SOUTH);
		ability_lvl_3_current_time = ability_lvl_3_cooldown;
		ability_lvl_3_ready = true;


	}
	radius_hit_lvl_3_particle = App->buff_manager->GetParticle(RADIUS_HIT_BIG_PARTICLE, NO_DIRECTION);

	skill_choosed[2] = true;
	ability[2] = choosed;
}

bool Wizard::PrepareAbility_lvl_3()
{
	if (ability_lvl_3_current_time < ability_lvl_3_cooldown)return false;

	ability_lvl_3_prepare_mode = true;

	//Focus the wizard in the attack direction
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint mouse_position = iPoint(x - App->render->camera.x, y - App->render->camera.y);
	direction_type = LookDirection(mouse_position, GetPositionRounded());
	CalculateSpecialAttackArea(mouse_position, false);



	if (ability[2])
	{
		std::vector<Unit*> units_in;
		uint size = App->entities_manager->units_quadtree.CollectCandidates(units_in, area_attack_spell_3);

		for (uint k = 0; k < size; k++)
		{
			if (units_in[k]->GetDiplomacy() == entity_diplomacy)continue;

			units_in[k]->SetBlitColor({ 255,150,150,255 });
		}
	}
	else {
		if (!App->entities_manager->GetDeathUnitList().empty())
		{
			std::list<Unit*> temp_list = App->entities_manager->GetDeathUnitList();

			std::list<Unit*>::iterator units_in = temp_list.begin();
			for (; units_in != temp_list.end(); units_in++)
			{
				Unit* current_unit = units_in._Ptr->_Myval;
				if (area_attack_spell_3.IsIn(&(current_unit->GetPosition())))
				{
					if (current_unit->GetDiplomacy() != entity_diplomacy || current_unit == this)continue;

					if (current_unit->GetAction() == DIE || current_unit->GetAction() == DISAPPEAR)
					{

						current_unit->SetBlitColor({ 200,200,150,255 });
					}
				}

			}
		}
	}
	return true;
}

void Wizard::Hability_lvl_3(int x, int y)
{
	ability_lvl_3_prepare_mode = false;

	if (!ability_lvl_3_ready)return;

	//Focus the wizard in the spell direction
	iPoint destination = iPoint(x, y);
	direction_type = LookDirection(destination, GetPositionRounded());
	if (!CalculateSpecialAttackArea(destination, false))
		return;
	action_type = ATTATCK;
	App->animator->UnitPlay(this);

	std::vector<Unit*> units_in;

	//Thunder skill
	if (ability[2])
	{
		ability_lvl_3_particle = App->buff_manager->GetParticle(THUNDER_PARTICLE, NO_DIRECTION);
		App->sound->PlayFXAudio(SOUND_TYPE::WIZARD_SKILL_LVL3_B);

		//Collect all the units in the buff area
		App->entities_manager->units_quadtree.CollectCandidates(units_in, area_attack_spell_3);
		uint size = units_in.size();
		for (uint k = 0; k < size; k++)
		{
			if (units_in[k]->GetDiplomacy() == entity_diplomacy)continue;

			units_in[k]->DirectDamage(ability_lvl_3_attack_value);
		}
	}

	//Resurrect skill
	else
	{
		//std::vector<Unit*> to_revive;
		std::list<Unit*> temp_list = App->entities_manager->GetDeathUnitList();
		std::list<Unit*>::iterator units_in = temp_list.begin();

		for (; units_in != temp_list.end(); units_in++)
		{
			Unit* current_unit = units_in._Ptr->_Myval;
			if (area_attack_spell_3.IsIn(&(current_unit->GetPosition())))
			{
				if (current_unit->GetDiplomacy() != entity_diplomacy)continue;

				if (current_unit->GetAction() == DIE || current_unit->GetAction() == DISAPPEAR)
				{
					current_unit->Resurrect();
					//	to_revive.push_back(current_unit);
				}
			}

		}
		/*
		while (!to_revive.empty())
		{
		to_revive.back()->Resurrect();
		to_revive.pop_back();
		}
		*/
		ability_lvl_3_particle = App->buff_manager->GetParticle(RESURRECTION_PARTICLE, NO_DIRECTION);
		App->sound->PlayFXAudio(SOUND_TYPE::WIZARD_SKILL_LVL3_A);

	}

	//Reset Particles
	ability_lvl_3_particle.animation.Reset();
	ability_lvl_3_particle.position = destination;


	actived[2] = true;
	ability_lvl_3_timer.Start();
	ability_lvl_3_current_time = 0;
	ability_lvl_3_ready = false;
}

void Wizard::CheckHability_lvl_3()
{
	if (!ability_lvl_3_particle.animation.IsEnd())
	{
		ability_lvl_3_particle.Draw();
	}
	else
	{
		actived[2] = false;
		action_type = IDLE;
		App->animator->UnitPlay(this);
	}
}

iPoint Wizard::GetiPointFromDirection(DIRECTION_TYPE direction) const
{
	switch (direction)
	{
	case NORTH:
		return iPoint(position.x, position.y - 1);
		break;
	case NORTH_EAST:
		return iPoint(position.x + 1, position.y - 1);
		break;
	case EAST:
		return iPoint(position.x + 1, position.y);
		break;
	case SOUTH_EAST:
		return iPoint(position.x + 1, position.y + 1);
		break;
	case SOUTH:
		return iPoint(position.x, position.y + 1);
		break;
	case SOUTH_WEST:
		return iPoint(position.x - 1, position.y + 1);
		break;
	case WEST:
		return iPoint(position.x - 1, position.y);
		break;
	case NORTH_WEST:
		return iPoint(position.x - 1, position.y - 1);
		break;
	}
	return iPoint(position.x - 1, position.y - 1);;
}

bool Wizard::CalculateSpecialAttackArea(const iPoint & base, bool attack_lvl_2)
{
	iPoint position = this->GetPositionRounded();

	if (attack_lvl_2)
	{
		int distance = position.DistanceTo(base);
		if (distance>area_limit_spell_2)
		{
			area_attack_spell_2.SetPosition(base);

			return false;
		}
		else
		{
		
			area_attack_spell_2.SetPosition(base);
			if (ability[1])
			{
				if (!App->entities_manager->CheckAreaToSpawn(area_attack_spell_2))
					return false;
			}
			return true;
		}
	}
	else
	{
		int distance = position.DistanceTo(base);

		if (distance>area_limit_spell_3)
		{
			area_attack_spell_3.SetPosition(base);
			return false;

		}
		else
		{
			area_attack_spell_3.SetPosition(base);
			return true;

		}
	}
}


void Wizard::SetPosition(float x, float y, bool insert)
{
	//Extract the units to push it with the new position later
	App->entities_manager->units_quadtree.Exteract(this, &position);

	//Set unit position
	position.x = x;
	position.y = y;
	iPoint pos(position.x, position.y);
	//Set unit vision position
	vision.SetPosition(pos);
	//Set unit render area position
	render_area.SetPosition(pos);
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

	//Check unit fog
	SDL_Point point = { pos.x,pos.y };
	int dist_mult = 10;
	if (SDL_PointInRect(&point, &App->render->camera_viewport))dist_mult = 2;
	if (entity_diplomacy == ALLY && (this->distance_walked.x == 0.00f || abs(distance_walked.x) + abs(distance_walked.y) > App->fog_of_war->alpha_cell_size * dist_mult))App->fog_of_war->CheckEntityFog(this);

	//Set area limit position
	//area_limit_spell_2.SetPosition(pos);
	//area_limit_spell_3.SetPosition(pos);

	//Add the unit with the correct position in the correct quad tree
	if (insert)App->entities_manager->units_quadtree.Insert(this, &position);
}

void Wizard::SetSpecialAttackArea(const Circle & circle, const char * name)
{
	if (strcmp("area_lvl_2", name) == 0)area_attack_spell_2 = circle;
	else if (strcmp("area_lvl_3", name) == 0)area_attack_spell_3 = circle;

}

void Wizard::SetAbility_lvl_2_HealValue(uint heal)
{
	ability_lvl_2_heal_value = heal;
}

void Wizard::SetAbility_lvl_3_AttackValue(uint attack)
{
	ability_lvl_3_attack_value = attack;
}

void Wizard::SetAreaLimitLvl2(int limit)
{
	area_limit_spell_2 = limit;
}
void Wizard::SetAreaLimitLvl3(int limit)
{
	area_limit_spell_3 = limit;
}



/// ---------------------------------------------
Hunter::Hunter() :Champion()
{

}

Hunter::Hunter(const Hunter & copy) : Champion(copy), area_triangle_attack_skill_A_lvl_2(copy.area_triangle_attack_skill_A_lvl_2), ability_lvl_2_skill_A_attack_value(copy.ability_lvl_2_skill_A_attack_value), area_attack_skill_B_lvl_2(copy.area_attack_skill_B_lvl_2), area_limit_skill_B_lvl_2(copy.area_limit_skill_B_lvl_2), ability_lvl_2_skill_B_attack_value(copy.ability_lvl_2_skill_B_attack_value),
area_attack_skill_A_lvl_3(copy.area_attack_skill_A_lvl_3), area_limit_skill_A_lvl_3(copy.area_limit_skill_A_lvl_3), ability_lvl_3_skill_A_attack_value(copy.ability_lvl_3_skill_A_attack_value), area_attack_skill_B_lvl_3(copy.area_attack_skill_B_lvl_3), ability_lvl_3_skill_B_attack_value(copy.ability_lvl_3_skill_B_attack_value)
{
	buff_to_apply = App->buff_manager->GetPassiveBuff(PASSIVE_BUFF, ATTACK_BUFF, false);
	ability_lvl_2_particle = App->buff_manager->GetParticle(SLASH_PARTICLE, SOUTH);
	ability_lvl_3_particle = App->buff_manager->GetParticle(SLASH_PARTICLE, SOUTH);
	radius_hit_lvl_2_particle = App->buff_manager->GetParticle(RADIUS_HIT_SMALL_PARTICLE, SOUTH);
	radius_hit_lvl_3_particle = App->buff_manager->GetParticle(RADIUS_HIT_BIG_PARTICLE, SOUTH);
	for (uint k = 0; k < 3; k++)
	{
		actived[k] = false;
		ability[k] = false;
		skill_choosed[k] = false;

	}
}


//Destructors =========================
Hunter::~Hunter()
{

}

void Hunter::SaveAsDef(pugi::xml_node & node)
{
	//Add all wizard attributes at the xml node

	//Name
	node.append_attribute("name") = "Hunter";

	/*Type*/	node.append_attribute("unit_type") = App->animator->EnumToUnitStr(unit_type);
	/*Class*/	node.append_attribute("unit_class") = App->animator->EnumToUnitClassStr(unit_class);
	/*Atk Type*/node.append_attribute("attack_type") = App->animator->EnumToAttackStr(attack_type);

	/*Selec X*/ node.append_attribute("selection_x") = selection_rect.x;
	/*Selec Y*/ node.append_attribute("selection_y") = selection_rect.y;
	/*Selec W*/ node.append_attribute("selection_w") = selection_rect.w;
	/*Selec H*/ node.append_attribute("selection_h") = selection_rect.h;

	/*Icon X*/	node.append_attribute("icon_x") = icon_rect.x;
	/*Icon Y*/	node.append_attribute("icon_y") = icon_rect.y;
	/*Icon W*/	node.append_attribute("icon_w") = icon_rect.w;
	/*Icon H*/	node.append_attribute("icon_h") = icon_rect.h;

	/*Vision rad*/	node.append_attribute("vision_rad") = vision.GetRad();
	/*Mark Rad*/	node.append_attribute("mark_rad") = mark.GetRad();
	/*Soft Rad*/	node.append_attribute("soft_rad") = soft_collider.GetRad();
	/*Hard Rad*/	node.append_attribute("hard_rad") = hard_collider.GetRad();

	/*Max Life*/	node.append_attribute("max_life") = max_life;
	/*Life Pos X*/	node.append_attribute("life_pos_x") = life_position.x;
	/*Life Pos Y*/	node.append_attribute("life_pos_y") = life_position.y;
	/*Speed*/		node.append_attribute("speed") = speed;
	/*Atk HitP*/	node.append_attribute("attack_hitpoints") = attack_hitpoints;
	/*Atk Rate*/	node.append_attribute("attack_rate") = attack_rate;
	/*Atk Range*/	node.append_attribute("attack_range") = attack_area.GetRad();
	/*Defense*/		node.append_attribute("defense") = defense;
	/*Food Cost*/	node.append_attribute("food_cost") = food_cost;
	/*Wood Cost*/	node.append_attribute("wood_cost") = wood_cost;
	/*Gold Cost*/	node.append_attribute("gold_cost") = gold_cost;
	/*Popu Cost*/	node.append_attribute("population_cost") = population_cost;
	/*Train Time*/	node.append_attribute("train_time") = train_time;
	/*Experience*/	node.append_attribute("unit_experience") = unit_experience;

	/*Atk for lvl*/			node.append_attribute("attack_for_level") = attack_for_level;
	/*Range for lvl*/		node.append_attribute("range_for_level") = range_for_level;
	/*Life for lvl*/		node.append_attribute("life_for_level") = life_for_level;
	/*Def for lvl*/			node.append_attribute("defense_for_level") = defense_for_level;
	/*Speed for lvl*/		node.append_attribute("speed_for_level") = speed_for_level;
	/*View for lvl*/		node.append_attribute("view_area_for_level") = view_area_for_level;

	/*Exp lvl 2*/			node.append_attribute("experience_lvl_2") = experience_lvl_2;
	/*Exp lvl 3*/			node.append_attribute("experience_lvl_3") = experience_lvl_3;
	
	/*Atk Tri Lenght Skill lvl 2 A*/node.append_attribute("area_triangle_length") = this->area_triangle_attack_skill_A_lvl_2.GetLength();
	/*Atk Tri Width Skill lvl 2 A*/	node.append_attribute("area_triangle_width_angle") = this->area_triangle_attack_skill_A_lvl_2.GetWidthAngle();
	/*Atk Skill lvl 2 A*/			node.append_attribute("ability_lvl_2_skill_A_attack_value") = ability_lvl_2_skill_A_attack_value;

	/*Area Atk Skill lvl 2 B*/		node.append_attribute("area_attack_skill_B_lvl_2") = area_attack_skill_B_lvl_2.GetRad();
	/*Area Limit Skill lvl 2 B*/	node.append_attribute("area_limit_skill_B_lvl_2") = area_limit_skill_B_lvl_2;
	/*Area Atk  Skilllvl 2 B*/		node.append_attribute("ability_lvl_2_skill_B_attack_value") = ability_lvl_2_skill_B_attack_value;
	
	/*Ab lvl 2 coold*/				node.append_attribute("ability_lvl_2_cooldown") = ability_lvl_2_cooldown;

	/*Area Atk Skill lvl 3 A*/		node.append_attribute("area_attack_skill_A_lvl_3") = area_attack_skill_A_lvl_3.GetRad();
	/*Area Limit Skill lvl 3 A*/	node.append_attribute("area_limit_skill_A_lvl_3") = area_limit_skill_A_lvl_3;
	/*Area Atk  Skill lvl 3 A*/		node.append_attribute("ability_lvl_3_skill_A_attack_value") = ability_lvl_3_skill_A_attack_value;
	
	/*Area Atk  Skill lvl 3 A*/		node.append_attribute("rect_attack_w") = area_attack_skill_B_lvl_3.GetWidth();
	/*Area Atk  Skill lvl 3 A*/		node.append_attribute("rect_attack_h") = area_attack_skill_B_lvl_3.GetHeight();
	/*Area Atk  Skill lvl 3 A*/		node.append_attribute("rect_pivot_distance") = area_attack_skill_B_lvl_3.GetPivotDistance();

	/*Ab lvl 2 coold*/				node.append_attribute("ability_lvl_3_cooldown") = ability_lvl_3_cooldown;

}



bool Hunter::Draw(bool debug)
{
	bool ret = false;
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint mouse_position(x - App->render->camera.x, y - App->render->camera.y);
	bool check = false;
	//Draw wizard Selection Mark
	if (selected)
	{
		if (ability_lvl_2_prepare_mode)
		{
			if (ability[1])
			{
				check = CalculateSpecialAttackArea(mouse_position, true);
				if (!check)
					radius_hit_lvl_2_particle.SetBlitColor({ 255,10,10,255 });
				radius_hit_lvl_2_particle.position = mouse_position;

				radius_hit_lvl_2_particle.Draw();
			}
			else
			{
				area_triangle_attack_skill_A_lvl_2.Draw();
			}

		}
		else if (ability_lvl_3_prepare_mode) {
			if (ability[2])
			{
				area_attack_skill_B_lvl_3.Draw();
			}
			else
			{
				check = CalculateSpecialAttackArea(mouse_position, false);
				if (!check)
					radius_hit_lvl_3_particle.SetBlitColor({ 255,10,10,255 });
				radius_hit_lvl_3_particle.position = mouse_position;
				radius_hit_lvl_3_particle.Draw();
				//	area_attack_skill_A_lvl_3.Draw();
			}
		}
		mark.Draw();
	}

	//Draw wizard logic areas
	if (debug && selected) {

		if (selected)
		{
			attack_area.Draw();
			soft_collider.Draw();
			hard_collider.Draw();
			vision.Draw();
			buff_area.Draw();
			area_attack_skill_B_lvl_3.Draw();
			area_triangle_attack_skill_A_lvl_2.Draw();

		}
	}

	//Calculate & draw wizard special attack area


	direction_type = LookDirection(mouse_position, GetPositionRounded());
	CalculateTriangleAttackArea(GetiPointFromDirection(direction_type));
	if (debug && selected) {
		//area_limit_spell_2.Draw();
		//area_limit_spell_3.Draw();

	}
	//Draw Entity Current animation frame
	const Sprite* sprite = current_animation->GetCurrentSprite();
	ret = App->render->CallBlit(current_animation->GetTexture(), position.x, position.y, sprite->GetFrame(), flip_sprite, -position.y - sprite->GetZ_cord(), sprite->GetOpacity(), sprite->GetXpivot(), sprite->GetYpivot(), &blit_color);

	return ret;
}


void Hunter::SetAbility_lvl_1(bool choosed)
{
	if (actived[0])return;

	if (choosed)
	{
		buff_to_apply = App->buff_manager->GetPassiveBuff(PASSIVE_BUFF, SPEED_BUFF, true);
	}
	else
	{
		buff_to_apply = App->buff_manager->GetPassiveBuff(PASSIVE_BUFF, RANGE_BUFF, true);
	}
	skill_choosed[0] = true;
	ability[0] = choosed;
}

void Hunter::Hability_lvl_1()
{

	if (actived[0])return;
	//Collect all the units in the buff area


	//Update units buffs (check for new units and old ones)
	App->buff_manager->CallBuff(this, buff_to_apply->GetBuffType(), buff_to_apply->GetAttributeType(), true);



	actived[0] = true;
}

void Hunter::CheckHability_lvl_1()
{
	//	App->buff_manager->CallBuff(this, buff_to_apply->GetBuffType(), buff_to_apply->GetAttributeType(), true);
}

void Hunter::SetAbility_lvl_2(bool choosed)
{
	if (level < 1) return;

	if (choosed)
	{
		radius_hit_lvl_2_particle = App->buff_manager->GetParticle(RADIUS_HIT_SMALL_PARTICLE, NO_DIRECTION);

		ability_lvl_2_particle = App->buff_manager->GetParticle(LONG_SHOT_PARTICLE, NO_DIRECTION);
		ability_lvl_2_attack_value = ability_lvl_2_skill_B_attack_value;
		ability_lvl_2_ready = true;
		
		
	}
	else
	{
		ability_lvl_2_particle = App->buff_manager->GetParticle(MULTI_SHOT_PARTICLE, SOUTH);
		ability_lvl_2_attack_value = ability_lvl_2_skill_A_attack_value;
		ability_lvl_2_ready = true;


	}
	skill_choosed[1] = true;

	ability[1] = choosed;
}

bool Hunter::PrepareAbility_lvl_2()
{
	if (!ability_lvl_2_ready)return false;
	ability_lvl_2_prepare_mode = true;
	//Focus the wizard in the attack direction
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint destination = iPoint(x - App->render->camera.x, y - App->render->camera.y);
	CalculateSpecialAttackArea(destination, true);
	direction_type = LookDirection(iPoint(x - App->render->camera.x, y - App->render->camera.y), GetPositionRounded());
	CalculateTriangleAttackArea(GetiPointFromDirection(direction_type));

	//Collect all the units in the buff area
	std::vector<Unit*> units_in;
	uint size = 0;
	if (ability[1])
	{


		size = App->entities_manager->units_quadtree.CollectCandidates(units_in, area_attack_skill_B_lvl_2);

	}
	else
	{
		size = App->entities_manager->units_quadtree.CollectCandidates(units_in, area_triangle_attack_skill_A_lvl_2);

	}


	for (uint k = 0; k < size; k++)
	{
		if (units_in[k]->GetDiplomacy() == entity_diplomacy)continue;
		units_in[k]->SetBlitColor({ 255,150,150,255 });
	}
	return true;

}

void Hunter::Hability_lvl_2(int x, int y)
{
	ability_lvl_2_prepare_mode = false;
	if (ability_lvl_2_current_time < ability_lvl_2_cooldown)return;

	//Focus the wizard in the spell direction
	iPoint destination = iPoint(x, y);
	direction_type = LookDirection(destination, GetPositionRounded());
	action_type = ATTATCK;
	App->animator->UnitPlay(this);
	std::vector<Unit*> units_in;

	if (ability[1])
	{
		if (!CalculateSpecialAttackArea(destination, true))
			return;
		App->sound->PlayFXAudio(SOUND_TYPE::HUNTER_SKILL_LVL2_B);
		//Collect all the units in the buff area
		App->entities_manager->units_quadtree.CollectCandidates(units_in, area_attack_skill_B_lvl_2);
		ability_lvl_2_particle.animation.Reset();
		ability_lvl_2_particle.position = area_attack_skill_B_lvl_2.GetPosition();
	}
	else
	{
		CalculateTriangleAttackArea(GetiPointFromDirection(direction_type));
		App->sound->PlayFXAudio(SOUND_TYPE::HUNTER_SKILL_LVL2_A);
		//Collect all the units in the buff area
		App->entities_manager->units_quadtree.CollectCandidates(units_in, area_triangle_attack_skill_A_lvl_2);
		ability_lvl_2_particle = App->buff_manager->GetParticle(MULTI_SHOT_PARTICLE, direction_type);
		ability_lvl_2_particle.animation.Reset();
		ability_lvl_2_particle.position = GetPositionRounded();
	}
	uint size = units_in.size();
	for (uint k = 0; k < size; k++)
	{
		if (units_in[k]->GetDiplomacy() == entity_diplomacy)continue;

		units_in[k]->DirectDamage(ability_lvl_2_attack_value);
	}

	actived[1] = true;


	ability_lvl_2_timer.Start();
	ability_lvl_2_ready = false;
	ability_lvl_2_current_time = 0;
}

void Hunter::CheckHability_lvl_2()
{
	if (!ability_lvl_2_particle.animation.IsEnd())
	{
		ability_lvl_2_particle.Draw();
	}
	else
	{
		actived[1] = false;
		action_type = IDLE;
		App->animator->UnitPlay(this);
	}
}
void Hunter::SetAbility_lvl_3(bool choosed)
{
	if (level < 2) return;

	if (choosed)
	{
		ability_lvl_3_particle = App->buff_manager->GetParticle(LASER_PARTICLE, NO_DIRECTION);
		ability_lvl_3_attack_value = ability_lvl_3_skill_B_attack_value;
		ability_lvl_3_ready = true; 
		
	}
	else
	{
		
		radius_hit_lvl_3_particle = App->buff_manager->GetParticle(RADIUS_HIT_BIG_PARTICLE, NO_DIRECTION);
		ability_lvl_3_particle = App->buff_manager->GetParticle(DRAGON_SHOT_PARTICLE, NO_DIRECTION);
		ability_lvl_3_attack_value = ability_lvl_3_skill_A_attack_value;
		ability_lvl_3_ready = true;

	}
	skill_choosed[2] = true;
	ability[2] = choosed;
}

bool Hunter::PrepareAbility_lvl_3()
{
	if (!ability_lvl_3_ready)return false;

	ability_lvl_3_prepare_mode = true;

	//Focus the wizard in the attack direction
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint mouse_position = iPoint(x - App->render->camera.x, y - App->render->camera.y);
	direction_type = LookDirection(mouse_position, GetPositionRounded());

	std::vector<Unit*> units_in;
	uint size = 0;
	if (ability[2])
	{
		CalculateArrorwAttackArea(mouse_position);
		size = App->entities_manager->units_quadtree.CollectCandidates(units_in, area_attack_skill_B_lvl_3);

	}
	else
	{
		CalculateSpecialAttackArea(mouse_position, false);
		size = App->entities_manager->units_quadtree.CollectCandidates(units_in, area_attack_skill_A_lvl_3);

	}
	for (uint k = 0; k < size; k++)
	{
		if (units_in[k]->GetDiplomacy() == entity_diplomacy)continue;
		units_in[k]->SetBlitColor({ 255,150,150,255 });
	}

	return true;


}

void Hunter::Hability_lvl_3(int x, int y)
{
	ability_lvl_3_prepare_mode = false;

	if (!ability_lvl_3_ready)return;

	//Focus the wizard in the spell direction
	iPoint destination = iPoint(x, y);
	direction_type = LookDirection(destination, GetPositionRounded());
	action_type = ATTATCK;
	App->animator->UnitPlay(this);
	std::vector<Unit*> units_in;

	if (ability[2])
	{
		CalculateArrorwAttackArea(destination);
		//long range skill
		App->sound->PlayFXAudio(SOUND_TYPE::HUNTER_SKILL_LVL3_B);
		//Collect all the units in the buff area
		App->entities_manager->units_quadtree.CollectCandidates(units_in, area_attack_skill_B_lvl_3);
		ability_lvl_3_particle.animation.Reset();
		ability_lvl_3_particle.position = this->GetPositionRounded();

		int deltaY = GetPositionRounded().y - y;
		int deltaX = GetPositionRounded().x - x;
		angle = atan2(deltaY, deltaX)*(180 / 3.14) + 180;

	}
	else
	{

		if (!CalculateSpecialAttackArea(destination, false))
			return;
		App->sound->PlayFXAudio(SOUND_TYPE::HUNTER_SKILL_LVL3_A);
		//Collect all the units in the buff area
		App->entities_manager->units_quadtree.CollectCandidates(units_in, area_attack_skill_A_lvl_3);
		ability_lvl_3_particle.animation.Reset();
		ability_lvl_3_particle.position = area_attack_skill_A_lvl_3.GetPosition();
	}
	uint size = units_in.size();
	for (uint k = 0; k < size; k++)
	{
		if (units_in[k]->GetDiplomacy() == entity_diplomacy)continue;

		if (ability[2])
		{
		
		units_in[k]->DirectDamage(ability_lvl_3_attack_value);
		}
		else {

		App->buff_manager->CallBuff(units_in[k], TIMED_BUFF, BURN_BUFF);
		units_in[k]->DirectDamage(ability_lvl_3_attack_value);

		}
	}

	actived[2] = true;

	ability_lvl_3_timer.Start();
	ability_lvl_3_current_time = 0;
	ability_lvl_3_ready = false;
}

void Hunter::CheckHability_lvl_3()
{
	if (!ability_lvl_3_particle.animation.IsEnd())
	{
			

	
		ability_lvl_3_particle.Draw(angle);
	}
	else
	{
		actived[2] = false;
		action_type = IDLE;
		App->animator->UnitPlay(this);
		angle = 0.0;
	}
}

iPoint Hunter::GetiPointFromDirection(DIRECTION_TYPE direction) const
{
	switch (direction)
	{
	case NORTH:
		return iPoint(position.x, position.y - 1);
		break;
	case NORTH_EAST:
		return iPoint(position.x + 1, position.y - 1);
		break;
	case EAST:
		return iPoint(position.x + 1, position.y);
		break;
	case SOUTH_EAST:
		return iPoint(position.x + 1, position.y + 1);
		break;
	case SOUTH:
		return iPoint(position.x, position.y + 1);
		break;
	case SOUTH_WEST:
		return iPoint(position.x - 1, position.y + 1);
		break;
	case WEST:
		return iPoint(position.x - 1, position.y);
		break;
	case NORTH_WEST:
		return iPoint(position.x - 1, position.y - 1);
		break;
	}
	return iPoint(position.x - 1, position.y - 1);;
}

void Hunter::CalculateTriangleAttackArea(const iPoint & base)
{
	//Update the triangle base
	area_triangle_attack_skill_A_lvl_2.SetBase(base);
	//Recalculate the triangle vertex with the new base
	area_triangle_attack_skill_A_lvl_2.CalculateVertex();
}

void Hunter::CalculateArrorwAttackArea(const iPoint & base)
{
	area_attack_skill_B_lvl_3.SetPosition(this->GetPositionRounded());
	area_attack_skill_B_lvl_3.SetGoal(iPoint(base.x, base.y));

	area_attack_skill_B_lvl_3.CalculateVertex();
}

bool Hunter::CalculateSpecialAttackArea(const iPoint & base, bool attack_lvl_2)
{
	iPoint position = this->GetPositionRounded();

	if (attack_lvl_2)
	{
		int distance = area_attack_skill_B_lvl_2.GetPosition().DistanceTo(position);
		if (distance>area_limit_skill_B_lvl_2)
		{
			area_attack_skill_B_lvl_2.SetPosition(base);

			return false;
		}
		else
		{
			area_attack_skill_B_lvl_2.SetPosition(base);
			return true;
		}
	}
	else
	{
		int distance = area_attack_skill_A_lvl_3.GetPosition().DistanceTo(position);

		if (distance>area_limit_skill_A_lvl_3)
		{
			area_attack_skill_A_lvl_3.SetPosition(base);
			return false;

		}
		else
		{
			area_attack_skill_A_lvl_3.SetPosition(base);
			return true;

		}
	}
}


void Hunter::SetPosition(float x, float y, bool insert)
{
	//Extract the units to push it with the new position later
	App->entities_manager->units_quadtree.Exteract(this, &position);

	//Set unit position
	position.x = x;
	position.y = y;
	iPoint pos(position.x, position.y);
	//Set unit vision position
	vision.SetPosition(pos);
	//Set unit render area position
	render_area.SetPosition(pos);
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

	//Check unit fog
	SDL_Point point = { pos.x,pos.y };
	int dist_mult = 10;
	if (SDL_PointInRect(&point, &App->render->camera_viewport))dist_mult = 2;
	if (entity_diplomacy == ALLY && (this->distance_walked.x == 0.00f || abs(distance_walked.x) + abs(distance_walked.y) > App->fog_of_war->alpha_cell_size * dist_mult))App->fog_of_war->CheckEntityFog(this);

	//Set area limit position
	//area_limit_spell_2.SetPosition(pos);
	//area_limit_spell_3.SetPosition(pos);
	CalculateArrorwAttackArea(pos);
	area_triangle_attack_skill_A_lvl_2.SetPosition(pos);

	//Add the unit with the correct position in the correct quad tree
	if (insert)App->entities_manager->units_quadtree.Insert(this, &position);
}

void Hunter::SetSpecialAttackArea(const Circle & circle, const char * name)
{
	if (strcmp("area_lvl_2", name) == 0)area_attack_skill_B_lvl_2 = circle;
	else if (strcmp("area_lvl_3", name) == 0)area_attack_skill_A_lvl_3 = circle;

}

void Hunter::SetSpecialAttackArea(const Triangle & triangle)
{
	area_triangle_attack_skill_A_lvl_2 = triangle;
}

void Hunter::SetSpecialAttackArea(const PivotedRect & rect)
{
	area_attack_skill_B_lvl_3 = rect;
}

void Hunter::SetAbility_lvl_2_A_AttackValue(uint attack)
{
	ability_lvl_2_skill_A_attack_value = attack;
}
void Hunter::SetAbility_lvl_2_B_AttackValue(uint attack)
{
	ability_lvl_2_skill_B_attack_value = attack;
}
void Hunter::SetAbility_lvl_3_A_AttackValue(uint attack)
{
	ability_lvl_3_skill_A_attack_value = attack;
}
void Hunter::SetAbility_lvl_3_B_AttackValue(uint attack)
{
	ability_lvl_3_skill_B_attack_value = attack;
}


void Hunter::SetAreaLimitLvl2(uint limit)
{
	area_limit_skill_B_lvl_2 = limit;
}
void Hunter::SetAreaLimitLvl3(uint limit)
{
	area_limit_skill_A_lvl_3 = limit;
}
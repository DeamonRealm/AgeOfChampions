#include "j1BuffManager.h"
#include "BaseEntities.h"
#include "j1App.h"
#include "j1FileSystem.h"
#include "p2Log.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "Champions.h"
#include "j1SoundManager.h"

// Buff definitions ----------------------------------------------------------------------------

/// Class Particle --------------------
//Constructors ==============
Particle::Particle()
{

}

Particle::Particle(const Particle & copy) :position(copy.position),z_depth(copy.z_depth), particle_type(copy.particle_type),direction_type(copy.direction_type), animation(copy.animation), flip_sprite(copy.flip_sprite)
{

}

//Destructors ===============
Particle::~Particle()
{
}

//Functionality =============
void Particle::Draw(double angle)
{
	Sprite* sprite = (Sprite*)animation.GetCurrentSprite();
	App->render->CallBlit(animation.GetTexture(), position.x, position.y, sprite->GetFrame(), flip_sprite, -position.y - z_depth, sprite->GetOpacity(), sprite->GetXpivot(), sprite->GetYpivot(),NULL,angle);
}

/// -----------------------------------


/// Class Buff Particle ---------------
//Constructors ==============
BuffParticle::BuffParticle()
{

}

BuffParticle::BuffParticle(const BuffParticle & copy) :Particle(copy), actor(copy.actor), buff_type(copy.buff_type), attribute_type(copy.attribute_type), draw_rate(copy.draw_rate)
{

}

//Destructors ===============
BuffParticle::~BuffParticle()
{

}

//Functionality =============
void BuffParticle::Draw(double angle)
{
	if (animation.IsEnd() && draw_timer.Read() < draw_rate)return;
	else if(animation.IsEnd())
	{
		animation.Reset();
		if (this->buff_type == BUFF_TYPE::PASSIVE_BUFF)
		{
			if (attribute_type == BUFF_ATTRIBUTE_TYPE::ATTACK_BUFF)App->sound->PlayFXAudio(SOUND_TYPE::WARRIOR_SKILL_LVL1_B);
			if (attribute_type == BUFF_ATTRIBUTE_TYPE::DEFENSE_BUFF)App->sound->PlayFXAudio(SOUND_TYPE::WARRIOR_SKILL_LVL1_A);
			if (attribute_type == BUFF_ATTRIBUTE_TYPE::LIFE_BUFF)App->sound->PlayFXAudio(SOUND_TYPE::WARRIOR_SKILL_LVL1_B);
			if (attribute_type == BUFF_ATTRIBUTE_TYPE::VISION_BUFF)App->sound->PlayFXAudio(SOUND_TYPE::WARRIOR_SKILL_LVL1_A);

		}
	}
	else
	{
		Sprite* sprite = (Sprite*)animation.GetCurrentSprite();
		App->render->CallBlit(animation.GetTexture(), position.x, position.y, sprite->GetFrame(), animation.GetLoop(), -position.y - z_depth, sprite->GetOpacity(), sprite->GetXpivot(), sprite->GetYpivot());
		draw_timer.Start();
	}

}

/// -----------------------------------

/// Class Passive Buff ----------------
// Constructors =============
PassiveBuff::PassiveBuff()
{

}

PassiveBuff::PassiveBuff(const PassiveBuff & copy) : buff_type(copy.buff_type), attribute_type(copy.attribute_type), value(copy.value), target(copy.target), actor(copy.actor), particle(copy.particle)
{

}

//Destructors ===============
PassiveBuff::~PassiveBuff()
{
	target = nullptr;
}

void PassiveBuff::Draw(double angle)
{
	if (particle.position != target->GetPositionRounded())particle.position = target->GetPositionRounded();
	particle.Draw();
}

//Set Methods =========================
void PassiveBuff::SetBuffType(BUFF_TYPE type)
{
	buff_type = type;
}

void PassiveBuff::SetAttributeType(BUFF_ATTRIBUTE_TYPE type)
{
	attribute_type = type;
}

void PassiveBuff::SetValue(float value)
{
	this->value = value;
}

void PassiveBuff::SetTarget(Unit * target)
{
	this->target = target;
}

void PassiveBuff::SetActor(bool act)
{
	actor = act;
}

void PassiveBuff::SetParticle(BuffParticle part)
{
	particle = part;
}

//Get Methods =========================
BUFF_TYPE PassiveBuff::GetBuffType() const
{
	return buff_type;
}

BUFF_ATTRIBUTE_TYPE PassiveBuff::GetAttributeType() const
{
	return attribute_type;
}

float PassiveBuff::GetValue() const
{
	return value;
}

Unit * PassiveBuff::GetTarget() const
{
	return target;
}

bool PassiveBuff::GetActor() const
{
	return actor;
}

//Functionality =======================
bool PassiveBuff::ApplyBuff()
{
	if (target == nullptr)
	{
		return false;
	}

	uint base = 0;
	float base_float = 0.0f;

	float buff = 0.0f;

	switch (attribute_type)
	{
	case ATTACK_BUFF:
		base = target->GetAttackHitPoints();
		buff = base * value;
		target->SetAttackBuff(buff);
		break;
	case SUPER_ATTACK_BUFF:
		base = target->GetAttackHitPoints();
		buff = base * value;
		target->SetAttackBuff(buff);
		break;
	case DEFENSE_BUFF:
		base = target->GetDefense();
		buff = base * value;
		target->SetDefenseBuff(buff);
		break;
	case VISION_BUFF:
	{
		base = target->GetVision().GetRad();
		buff = base * value;
		target->SetVisionBuff(buff);
		Circle tmp = target->GetVision();
		tmp.SetRad(tmp.GetRad() + target->GetVisionBuff());
		target->SetVision(tmp);
		break;
	}
	case LIFE_BUFF:
		base = target->GetBaseMaxLife();
		buff = base * value;
		target->SetLifeBuff(buff);
		break;
	case SPEED_BUFF:
		base = target->GetAttackRateBuff();
		buff = base * value;
		target->SetAttackRateBuff(buff);
		break;
	case RANGE_BUFF:
	{
		base = target->GetAttackArea()->GetRad();
		buff = base * value;
		Circle tmp = *target->GetAttackArea();
		target->SetAttackAreaBuff(buff);
		tmp.SetRad(base + buff);
		break;
	}
	case BURN_BUFF:
		base_float = target->GetSpeedBuff();
		buff = base_float * value;
		target->SetLifeBuff(buff);
		break;
	}


	particle.draw_timer.Start();
	particle.animation.Reset();

	return true;
}

bool PassiveBuff::RemoveBuff()
{
	if (target == nullptr)
	{
		return false;
	}

	uint base=0;
	float base_float=0.0f;
	float current_buff=0.0f;

	switch (attribute_type)
	{
	case ATTACK_BUFF:
		base = target->GetAttackHitPoints();
		current_buff = base * value;
		target->SetAttackBuff(-current_buff);
		break;
	case SUPER_ATTACK_BUFF:
		base = target->GetAttackHitPoints();
		current_buff = base * value;
		target->SetAttackBuff(-current_buff);
		break;
	case DEFENSE_BUFF:
		base = target->GetDefense();
		current_buff = base * value;
		target->SetDefenseBuff(-current_buff);
		break;
	case VISION_BUFF:
	{
		LOG("vision_buff %i", target->GetVisionBuff());
		Circle tmp = target->GetVision();
		tmp.SetRad(tmp.GetRad() - target->GetVisionBuff());
		target->SetVisionBuff(0);
		LOG("vision_buff %i", target->GetVisionBuff());

		target->SetVision(tmp);	
		break;
	}
	case LIFE_BUFF:
		base = target->GetBaseMaxLife();
		current_buff = base * value;
		target->SetLifeBuff(-current_buff);
		break;
	case TAUNT_BUFF:
		target->QuitProtection();
		break;
	case SPEED_BUFF:
		base = target->GetAttackRate();
		current_buff = base * value;
		target->SetAttackRateBuff(-current_buff);
		break;
	case RANGE_BUFF:
	{
		base = target->GetAttackArea()->GetRad();
		int buff = target->GetAttackAreaBuff();
		Circle tmp = *target->GetAttackArea();
		tmp.SetRad(base - buff);
		current_buff = base * value;
		target->SetAttackAreaBuff(-current_buff);	
		break;
	}
		break;
	case BURN_BUFF:
		base_float = target->GetSpeed();
		current_buff = base_float * value;
		target->SetSpeedBuff(-current_buff);
		break;
	}

	return true;
}

bool PassiveBuff::operator==(const PassiveBuff& tar)const
{
	return (value == tar.value && buff_type == tar.buff_type && attribute_type == tar.attribute_type);
}
/// -----------------------------------

/// Class Buff ------------------------
//Constructor =========================
Buff::Buff()
{

}

Buff::Buff(const Buff & copy) :PassiveBuff(copy), buff_duration(copy.buff_duration)
{

}

//Destructor ==========================
Buff::~Buff()
{

}

//Set Methods =========================
void Buff::SetBuffDuration(uint buff_duration)
{
	this->buff_duration = buff_duration;
}

//Get Methods =========================
uint Buff::GetBuffDuration() const
{
	return buff_duration;
}

//Functionality =======================
bool Buff::ApplyBuff()
{
	if (target == nullptr)
	{
		return false;
	}

	uint base=0;
	float base_float=0.0f;
	float buff = 0.0f;

	switch (attribute_type)
	{
	case ATTACK_BUFF:
		base = target->GetAttackHitPoints();
		buff = base * value;
		target->SetAttackBuff(buff);
		break;
	case SUPER_ATTACK_BUFF:
		base = target->GetAttackHitPoints();
		buff = base * value;
		target->SetAttackBuff(buff);
		break;
	case DEFENSE_BUFF:
		base = target->GetDefense();
		buff = base * value;
		target->SetDefenseBuff(buff);
		break;
	case VISION_BUFF:
	{
		base = target->GetVision().GetRad();
		buff = base * value;
		target->SetVisionBuff(buff);
		Circle tmp = target->GetVision();
		tmp.SetRad(tmp.GetRad() + target->GetVisionBuff());
		target->SetVision(tmp);
	}
		break;
	case LIFE_BUFF:
		base = target->GetBaseMaxLife();
		buff = base * value;
		target->SetLifeBuff(buff);
		break;
	case SPEED_BUFF:
		base = target->GetAttackRateBuff();
		buff = base * value;
		target->SetAttackRateBuff(buff);
		break;
	case RANGE_BUFF:
	{
		base = target->GetAttackArea()->GetRad();
		buff = base * value;
		Circle tmp = *target->GetAttackArea();
		target->SetAttackAreaBuff(buff);
		tmp.SetRad(base + buff);
		break;
	}
	case BURN_BUFF:
		base_float = target->GetSpeedBuff();
		buff = base_float * value;
		target->SetLifeBuff(buff);
		break;
	}

	timer.Start();
	return true;
}

bool Buff::IsActive() const
{
	return (timer.Read() < buff_duration);
}

void Buff::Reset()
{
	timer.Start();
	particle.animation.Reset();
}


// j1BuffModule -------------------------------------------------------------------

j1BuffManager::j1BuffManager() 
{
	name = "buff_manager";
}

j1BuffManager::~j1BuffManager()
{

}

void j1BuffManager::Enable()
{
	//Load all buffs definitions
	pugi::xml_document buffs_data;
	std::string load_folder = name + "/" + "buff_definitions.xml";
	if (!App->fs->LoadXML(load_folder.c_str(), &buffs_data))
	{
		LOG("Buffs definitions load failed!");
	}

	//Load all the buff particles to use it in buffs
	pugi::xml_node particle_document_node = buffs_data.first_child().first_child();

	//Iterate all the buff particles definitions
	while (particle_document_node != NULL)
	{
		//Load particle animation document
		pugi::xml_document particle_data;
		load_folder = name + "/" + particle_document_node.attribute("document").as_string();
		if (!App->fs->LoadXML(load_folder.c_str(), &particle_data))
		{
			LOG("Buff animation load failed!");
		}

		//Focus the first load document particle

		pugi::xml_node particle_node = particle_data.first_child().first_child();
		bool simple_particle = false;

		//Load particle texture
		load_folder = name + "/" + particle_data.first_child().attribute("imagePath").as_string();
		SDL_Texture* particle_texture = App->tex->Load(load_folder.c_str());

		//Iterate all the particles in the loaded document
		while (particle_node != NULL)
		{
			simple_particle = (strcmp(particle_node.name(), "simple_particle") == 0);

			Particle* particle_def = nullptr;
			if (!simple_particle)
			{
				particle_def = new BuffParticle();

				/*Set Buff Type*/		((BuffParticle*)particle_def)->buff_type = StrToBuffType(particle_node.attribute("buff_type").as_string());
				/*Set Attribute Type*/	((BuffParticle*)particle_def)->attribute_type = StrToBuffAttributeType(particle_node.attribute("attribute_type").as_string());
				/*Set actor*/			((BuffParticle*)particle_def)->actor = particle_node.attribute("actor").as_bool();
				/*Set Draw rate*/		((BuffParticle*)particle_def)->draw_rate = particle_node.attribute("draw_rate").as_uint();

			}
			else
			{
				particle_def = new Particle();

			}

			/*Set Particle Type*/	particle_def->particle_type = StrToParticleType(particle_node.attribute("particle_type").as_string());
			/*Set Particle Dir*/	particle_def->direction_type = App->animator->StrToDirectionEnum(particle_node.attribute("direction").as_string());
			/*Set z depth*/			particle_def->z_depth = particle_node.attribute("z").as_int();

			//Build particle animation
			Animation animation;

			//Focus the first animation child where basic stats are defined
			pugi::xml_node anim_node = particle_node.first_child();

			//Set basic stats
			animation.SetTexture(particle_texture);
			animation.SetSpeed(anim_node.attribute("speed").as_uint());
			animation.SetLoop(anim_node.attribute("loop").as_bool());

			//Focus the node to the next sprite
			anim_node = anim_node.next_sibling();
			while (anim_node != NULL)
			{
				//Load sprite rect
				SDL_Rect rect = { anim_node.attribute("x").as_int(),anim_node.attribute("y").as_int(),anim_node.attribute("w").as_int(),anim_node.attribute("h").as_int() };
				//Load sprite pivot
				float pX = anim_node.attribute("pX").as_float() * rect.w;
				pX = (pX > (floor(pX) + 0.5f)) ? ceil(pX) : floor(pX);
				float pY = anim_node.attribute("pY").as_float() * rect.h;
				pY = (pY > (floor(pY) + 0.5f)) ? ceil(pY) : floor(pY);
				//Load sprite opacity
				uint opacity = anim_node.attribute("opacity").as_uint();

				//Add sprite at animation
				animation.AddSprite(rect, iPoint(pX, pY), anim_node.attribute("z").as_int(), opacity);

				//Focus next animation sprite
				anim_node = anim_node.next_sibling();
			}

			//Set the animation built at the particle
			particle_def->animation = animation;

			//Add the definition in the buff manager
			if (!simple_particle)buff_particle_definitions.push_back((BuffParticle*)particle_def);
			else particle_definitions.push_back(particle_def);

			//Focus the next particle node
			particle_node = particle_node.next_sibling();
		}

		//Focus the next document node to load a new definition
		particle_document_node = particle_document_node.next_sibling();

		//Check if the next node is a particle node or a buff node
		if (strcmp(particle_document_node.name(), "particle") != 0)break;
	}

	//Point to the first buff definition
	pugi::xml_node buff_node = buffs_data.first_child().child("buff");

	PassiveBuff* buff_def = nullptr;

	//Iterate all the buffs definitions
	while (buff_node != NULL)
	{
		//Get the current buff type
		BUFF_TYPE buff_type = StrToBuffType(buff_node.attribute("buff_type").as_string());

		//Check the buff type to allocate the correct class
		if (buff_type == PASSIVE_BUFF)
		{
			buff_def = new PassiveBuff();
		}
		else
		{
			buff_def = new Buff();
		}

		//Build the buff base definition
		/*Buff Type*/	buff_def->SetBuffType(buff_type);
		/*Atr.Type*/	buff_def->SetAttributeType(StrToBuffAttributeType(buff_node.attribute("attribute_type").as_string()));
		/*Value*/		buff_def->SetValue(buff_node.attribute("value").as_float());
		/*Actor*/		buff_def->SetActor(buff_node.attribute("actor").as_bool());
		/*Animation*/	buff_def->SetParticle(GetBuffParticle(buff_def->GetBuffType(), buff_def->GetAttributeType(), buff_def->GetActor()));

		if (buff_type == TIMED_BUFF)
		{
			/*Buff Duration*/	((Buff*)buff_def)->SetBuffDuration(buff_node.attribute("buff_duration").as_uint());
		}


		//Add the new definition in the vector
		buff_definitions.push_back(buff_def);

		//Get the next xml node 
		buff_node = buff_node.next_sibling();
	}

}

void j1BuffManager::Disable()
{
	CleanUp();
}

bool j1BuffManager::Start()
{

	return true;
}

bool j1BuffManager::Update(float dt)
{
	//Update (draw & clean) all the timed buffs
	std::list<Buff*>::iterator item = active_buffs.begin();
	while (item != active_buffs.end())
	{
		Buff* current_buff = item._Ptr->_Myval;
		if (!current_buff->IsActive())
		{
			current_buff->RemoveBuff();
			active_buffs.remove(current_buff);
			RELEASE(current_buff);
		}
		else
		{
			current_buff->Draw();
		}

		item++;
	}

	//Draw all passive buffs (statics)
	std::list<PassiveBuff*>::const_iterator static_item = static_buffs.begin();
	while (static_item != static_buffs.end())
	{
		static_item._Ptr->_Myval->Draw();

		static_item++;
	}

	return true;
}

bool j1BuffManager::CleanUp()
{	
	//RELEASE buff_definitions
	uint size = buff_definitions.size();
	for (int i = 0; i < size; i++)
	{
		buff_definitions[i]->particle.animation.ReleaseTexture();
		RELEASE(buff_definitions[i]);
	}
	buff_definitions.clear();
	
	//RELEASE particle_definitions
	size = particle_definitions.size();
	for (int i = 0; i < size; i++)
	{
		particle_definitions[i]->animation.ReleaseTexture();
		RELEASE(particle_definitions[i]);
	}
	particle_definitions.clear();
	
	//RELEASE buff_particle_definitions
	size = buff_particle_definitions.size();
	for (int i = 0; i < size; i++)
	{
		buff_particle_definitions[i]->animation.ReleaseTexture();
		RELEASE(buff_particle_definitions[i]);
	}
	buff_particle_definitions.clear();
	
	//RELEASE active_buffs
	for (std::list<Buff*>::iterator item = active_buffs.begin(); item != active_buffs.end(); item++)
	{
		RELEASE(item._Ptr->_Myval);
	}
	active_buffs.clear();
	
	//RELEASE static_buffs
	for (std::list<PassiveBuff*>::iterator item = static_buffs.begin(); item != static_buffs.end(); item++)
	{
		RELEASE(item._Ptr->_Myval);
	}
	static_buffs.clear();

	return true;
}

//Methods to transform loaded string to buffs enums
BUFF_TYPE j1BuffManager::StrToBuffType(const char * str) const
{
	if (strcmp("passive",str) == 0)	return BUFF_TYPE::PASSIVE_BUFF;
	if (strcmp("timed",str) == 0)	return BUFF_TYPE::TIMED_BUFF;
	return NO_BUFF;
}

BUFF_ATTRIBUTE_TYPE j1BuffManager::StrToBuffAttributeType(const char * str) const
{
	if (strcmp("attack", str) == 0) 		return BUFF_ATTRIBUTE_TYPE::ATTACK_BUFF;
	if (strcmp("super_attack", str) == 0) 	return BUFF_ATTRIBUTE_TYPE::SUPER_ATTACK_BUFF;
	if (strcmp("defense", str) == 0)		return BUFF_ATTRIBUTE_TYPE::DEFENSE_BUFF;
	if (strcmp("vision", str) == 0)			return BUFF_ATTRIBUTE_TYPE::VISION_BUFF;
	if (strcmp("life", str) == 0)			return BUFF_ATTRIBUTE_TYPE::LIFE_BUFF;
	if (strcmp("stun", str) == 0)			return BUFF_ATTRIBUTE_TYPE::STUN_BUFF;
	if (strcmp("taunt", str) == 0)			return BUFF_ATTRIBUTE_TYPE::TAUNT_BUFF;
	if (strcmp("speed_attack", str) == 0)	return BUFF_ATTRIBUTE_TYPE::SPEED_BUFF;
	if (strcmp("range_attack", str) == 0)	return BUFF_ATTRIBUTE_TYPE::RANGE_BUFF;
	if (strcmp("burn", str) == 0)			return BUFF_ATTRIBUTE_TYPE::BURN_BUFF;

	return NO_ATTRIBUTE;
}

PARTICLE_TYPE j1BuffManager::StrToParticleType(const char * str) const
{
	if (strcmp("buff_particle", str) == 0) 				return PARTICLE_TYPE::BUFF_PARTICLE;
	if (strcmp("slash_particle", str) == 0)				return PARTICLE_TYPE::SLASH_PARTICLE;
	if (strcmp("stun_particle", str) == 0)				return PARTICLE_TYPE::STUN_PARTICLE;
	if (strcmp("taunt_particle", str) == 0)				return PARTICLE_TYPE::TAUNT_PARTICLE;
	if (strcmp("teleport_particle", str) == 0)			return PARTICLE_TYPE::TELEPORT_PARTICLE;
	if (strcmp("heal_particle", str) == 0)				return PARTICLE_TYPE::HEAL_PARTICLE;
	if (strcmp("thunder_particle", str) == 0)			return PARTICLE_TYPE::THUNDER_PARTICLE;
	if (strcmp("multi_shot_particle", str) == 0)		return PARTICLE_TYPE::MULTI_SHOT_PARTICLE;
	if (strcmp("dragon_shot_particle", str) == 0)		return PARTICLE_TYPE::DRAGON_SHOT_PARTICLE;
	if (strcmp("resurrection_particle", str) == 0)		return PARTICLE_TYPE::RESURRECTION_PARTICLE;
	if (strcmp("lvlup_particle", str) == 0)				return PARTICLE_TYPE::LEVEL_UP_PARTICLE;
	if (strcmp("one_hit_particle", str) == 0)			return PARTICLE_TYPE::ONE_HIT_PARTICLE;
	if (strcmp("laser_particle", str) == 0)				return PARTICLE_TYPE::LASER_PARTICLE;
	if (strcmp("long_shoot_particle", str) == 0)		return PARTICLE_TYPE::LONG_SHOT_PARTICLE;

	
	return UNKNOWN_PARTICLE;
}

BuffParticle j1BuffManager::GetBuffParticle(BUFF_TYPE bf_type, BUFF_ATTRIBUTE_TYPE atr_type, bool act)
{
	uint size = buff_particle_definitions.size();
	for (uint k = 0; k < size; k++)
	{
		if (buff_particle_definitions[k]->buff_type == bf_type && buff_particle_definitions[k]->attribute_type == atr_type && buff_particle_definitions[k]->actor == act)
		{
			return BuffParticle(*buff_particle_definitions[k]);
		}
	}
	return BuffParticle(*buff_particle_definitions[0]);
}

bool j1BuffManager::CallBuff(Unit * target, BUFF_TYPE buff_type, BUFF_ATTRIBUTE_TYPE buff_atr_type, bool actor)
{
	if (target == nullptr)
	{
		return false;
	}

	uint size = buff_definitions.size();

	for(uint k = 0; k < size; k++)
	{
		if (buff_definitions[k]->GetBuffType() == buff_type && buff_definitions[k]->GetAttributeType() == buff_atr_type && buff_definitions[k]->GetActor() == actor)
		{
			//Build a new buff with the selected target & states
			PassiveBuff* buff = nullptr;
			if (buff_type == TIMED_BUFF)
			{
				buff = new Buff((*(Buff*)buff_definitions[k]));
			}
			else
			{
				buff = new PassiveBuff(*buff_definitions[k]);
			}

			//Apply the buff
			buff->SetTarget(target);
			buff->ApplyBuff();

			//Add the buff at the active buffs list if is a timed buff
			if (buff_type == TIMED_BUFF)
			{
				((Buff*)buff)->Reset();
				active_buffs.push_back(((Buff*)buff));
			}
			else static_buffs.push_back(buff);

			return true;
		}
	}

	return false;
}

PassiveBuff * j1BuffManager::GetPassiveBuff(BUFF_TYPE buff_type, BUFF_ATTRIBUTE_TYPE buff_atr_type, bool actor)
{
	uint size = buff_definitions.size();

	for (uint k = 0; k < size; k++)
	{
		if (buff_definitions[k]->GetBuffType() == buff_type && buff_definitions[k]->GetAttributeType() == buff_atr_type && buff_definitions[k]->GetActor() == actor)
		{
			return buff_definitions[k];
		}
	}
	return nullptr;
}

void j1BuffManager::RemoveTargetBuff(Unit * target, PassiveBuff * buff)
{
	std::list<PassiveBuff*>::iterator item = static_buffs.begin();
	while (item != static_buffs.end())
	{
		if (item._Ptr->_Myval->GetTarget() == target && *item._Ptr->_Myval == *buff)
		{
			PassiveBuff* bf = item._Ptr->_Myval;

			//Clean the target buff effects
			bf->RemoveBuff();

			//Delete the buff
			static_buffs.remove(bf);
			RELEASE(bf);

			return;
		}

		item++;
	}
}

void j1BuffManager::RemoveTargetBuffs(Unit* target)
{
	if (target->GetUnitType() == WARRIOR_CHMP)
	{
		((Champion*)target)->CleanBuffedUnits();
		((Warrior*)target)->ClearProtectedUnits();
	}
	else if (target->GetUnitType() == WIZARD_CHMP)
	{
		((Champion*)target)->CleanBuffedUnits();
	}
	else if (target->GetUnitType() == ARCHER_CHMP)
	{
		((Champion*)target)->CleanBuffedUnits();
	}
	std::list<PassiveBuff*>::iterator item = static_buffs.begin();
	while (item != static_buffs.end())
	{
		if (item._Ptr->_Myval->GetTarget() == target)
		{
			PassiveBuff* bf = item._Ptr->_Myval;

			//Clean the target buff effects
			bf->RemoveBuff();

			//Delete the buff
			static_buffs.remove(bf);
			RELEASE(bf);

			return;
		}

		item++;
	}
}

Particle j1BuffManager::GetParticle(PARTICLE_TYPE ty, DIRECTION_TYPE direc)
{
	DIRECTION_TYPE real_dir = direc;
	bool flip = false;
	switch (direc)
	{

	case NORTH_EAST:
		real_dir = NORTH_WEST;
		flip = true;
		break;
	case EAST:
		real_dir = WEST;
		flip = true;
		break;
	case SOUTH_EAST:
		real_dir = SOUTH_WEST;
		flip = true;
		break;
	}

	uint size = particle_definitions.size();
	for (uint k = 0; k < size; k++)
	{
		if (particle_definitions[k]->particle_type == ty && particle_definitions[k]->direction_type == real_dir)
		{
			Particle particle(*particle_definitions[k]);
			particle.flip_sprite = flip;
			return particle;
		}
	}
	return Particle();
}

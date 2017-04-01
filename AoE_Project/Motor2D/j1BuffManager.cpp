#include "j1BuffManager.h"
#include "BaseEntities.h"
#include "j1App.h"
#include "j1FileSystem.h"
#include "p2Log.h"
#include "j1Render.h"
#include "j1Textures.h"

// Buff definitions ----------------------------------------------------------------------------

/// Class Buff Particle ---------------
//Constructors ==============
BuffParticle::BuffParticle()
{

}

BuffParticle::BuffParticle(const BuffParticle & copy) :position(copy.position), actor(copy.actor), buff_type(copy.buff_type), attribute_type(copy.attribute_type), draw_rate(copy.draw_rate), animation(copy.animation)
{

}

//Destructors ===============
BuffParticle::~BuffParticle()
{

}

//Functionality =============
void BuffParticle::Draw()
{
	if (draw_timer.Read() > draw_rate || !animation.IsEnd())
	{
		Sprite* sprite = (Sprite*)animation.GetCurrentSprite();
		App->render->Blit(animation.GetTexture(), position.x, position.y, sprite->GetFrame(), animation.GetLoop(), sprite->GetOpacity(), sprite->GetXpivot(), sprite->GetYpivot());
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

void PassiveBuff::Draw()
{
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

	uint base;
	float buff;

	switch (attribute_type)
	{
	case ATTACK_BUFF:
		base = target->GetAttackHitPoints();
		buff = base * value;
		target->SetAttackBuff(buff);
		break;

	case DEFENSE_BUFF:
		base = target->GetDefense();
		buff = base * value;
		target->SetDefenseBuff(buff);
		break;
	}

	return true;
}

bool PassiveBuff::RemoveBuff()
{
	if (target == nullptr)
	{
		return false;
	}

	uint base;
	float current_buff;
	float buff;

	switch (attribute_type)
	{
	case ATTACK_BUFF:
		base = target->GetAttackHitPoints();
		current_buff = base * value;
		target->SetAttackBuff(MAX(target->GetAttackBuff() - current_buff, 0));
		break;

	case DEFENSE_BUFF:
		base = target->GetDefense();
		current_buff = base * value;
		target->SetDefenseBuff(MAX(target->GetDefenseBuff() - current_buff, 0));
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

	uint base;
	float buff;

	switch (attribute_type)
	{
	case ATTACK_BUFF:
		base = target->GetAttackHitPoints();
		buff = base * value;
		target->SetAttackBuff(buff);
		break;

	case DEFENSE_BUFF:
		base = target->GetDefense();
		buff = base * value;
		target->SetDefenseBuff(buff);
		break;
	}

	timer.Start();
	return true;
}

bool Buff::IsActive() const
{
	return (timer.Read() < buff_duration);
}


// j1BuffModule -------------------------------------------------------------------

j1BuffManager::j1BuffManager() 
{
	name = "buff_manager";
}

j1BuffManager::~j1BuffManager()
{

}

bool j1BuffManager::Start()
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

		//Load particle texture
		load_folder = name + "/" + particle_data.first_child().attribute("imagePath").as_string();
		SDL_Texture* particle_texture = App->tex->Load(load_folder.c_str());

		//Iterate all the particles in the loaded document
		while (particle_node != NULL)
		{
			BuffParticle* buff_particle_deff = new BuffParticle();

			/*Set Buff Type*/		buff_particle_deff->buff_type = StrToBuffType(particle_node.attribute("buff_type").as_string());
			/*Set Attribute Type*/	buff_particle_deff->attribute_type = StrToBuffAttributeType(particle_node.attribute("attribute_type").as_string());
			/*Set actor*/			buff_particle_deff->actor = particle_node.attribute("actor").as_bool();
			/*Set Draw rate*/		buff_particle_deff->draw_rate = particle_node.attribute("draw_rate").as_uint();
			
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
			buff_particle_deff->animation = animation;

			//Add the definition in the buff manager
			buff_particle_definitions.push_back(buff_particle_deff);

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

		if(buff_type == TIMED_BUFF)
		{
			/*Buff Duration*/	((Buff*)buff_def)->SetBuffDuration(buff_node.attribute("buff_duration").as_uint());
		}


		//Add the new definition in the vector
		buff_definitions.push_back(buff_def);

		//Get the next xml node 
		buff_node = buff_node.next_sibling();
	}

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
		else current_buff->Draw();

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
	if (strcmp("attack", str) == 0) 	return BUFF_ATTRIBUTE_TYPE::ATTACK_BUFF;
	if (strcmp("defense", str) == 0)	return BUFF_ATTRIBUTE_TYPE::DEFENSE_BUFF;
	return NO_ATTRIBUTE;
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

bool j1BuffManager::CallBuff(Unit * target, BUFF_TYPE buff_type, BUFF_ATTRIBUTE_TYPE buff_atr_type)
{
	if (target == nullptr)
	{
		return false;
	}

	uint size = buff_definitions.size();

	for(uint k = 0; k < size; k++)
	{
		if (buff_definitions[k]->GetBuffType() == buff_type && buff_definitions[k]->GetAttributeType() == buff_atr_type)
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
			if (buff_type == TIMED_BUFF)active_buffs.push_back(((Buff*)buff));
			else static_buffs.push_back(buff);

			return true;
		}
	}

	return false;
}

void j1BuffManager::RemoveTargetBuff(Unit * target, PassiveBuff * buff)
{
	std::list<PassiveBuff*>::iterator item = static_buffs.begin();
	while (item != static_buffs.end())
	{
		if (item._Ptr->_Myval->GetTarget() == target /*&& item._Ptr->_Myval == buff*/)
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

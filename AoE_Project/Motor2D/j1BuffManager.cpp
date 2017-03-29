#include "j1BuffManager.h"
#include "BaseEntities.h"
#include "j1App.h"
#include "j1FileSystem.h"
#include "p2Log.h"

// Buff definitions ----------------------------------------------------------------------------

/// Class Passive Buff ----------------
// Constructors =============
PassiveBuff::PassiveBuff()
{

}

PassiveBuff::PassiveBuff(const PassiveBuff & copy): buff_type(copy.buff_type),attribute_type(copy.attribute_type),value(copy.value),target(copy.target)
{

}

//Destructors ===============
PassiveBuff::~PassiveBuff()
{
	target = nullptr;
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

	//Point to the first buff definition
	pugi::xml_node buff_node = buffs_data.first_child().first_child();

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

		item++;
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

			return true;
		}
	}

	return false;
}



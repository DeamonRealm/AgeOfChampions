#include "j1BuffManager.h"
#include "BaseEntities.h"

// Buff definitions ----------------------------------------------------------------------------

Buff::Buff()
{

}

Buff::Buff(const Buff & copy): type(copy.type), value(copy.value), buff_duration(copy.buff_duration)
{

}

Buff::~Buff()
{

}

void Buff::SetType(BUFF_TYPE type)
{
	this->type = type;
}

void Buff::SetValue(float value)
{
	this->value = value;
}

void Buff::SetBuffDuration(uint buff_duration)
{
	this->buff_duration = buff_duration;
}

void Buff::SetTarget(Unit * target)
{
	this->target = target;
}

BUFF_TYPE Buff::GetBuffType() const
{
	return type;
}

float Buff::GetValue() const
{
	return value;
}

uint Buff::GetBuffDuration() const
{
	return buff_duration;
}

Unit * Buff::GetTarget() const
{
	return target;
}

bool Buff::ApplyBuff()
{
	if (target == nullptr)
	{
		return false;
	}

	uint base;
	float buff;

	switch (type)
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

bool Buff::RemoveBuff()
{
	if (target == nullptr)
	{
		return false;
	}

	uint base;
	float current_buff;
	float buff;

	switch (type)
	{
	case ATTACK_BUFF:
		base = target->GetAttackHitPoints();
		current_buff = base * value;
		target->SetAttackBuff(MAX(target->GetAttackBuff() - current_buff,0));
		break;

	case DEFENSE_BUFF:
		base = target->GetDefense();
		current_buff = base * value;
		target->SetDefenseBuff(MAX(target->GetDefenseBuff() - current_buff, 0));
		break;
	}

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

bool j1BuffManager::CallBuff(Unit * target, BUFF_TYPE type)
{
	if (target == nullptr)
	{
		return false;
	}

	Buff* new_buff = new Buff();
	new_buff->SetTarget(target);
	new_buff->SetType(type);
	active_buffs.push_back(new_buff);
	new_buff->ApplyBuff();

	return true;
}

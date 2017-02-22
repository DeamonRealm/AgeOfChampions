#include "BaseEntities.h"

#include "j1App.h"
#include "j1Render.h"
#include "j1Animator.h"

#include "Iso_Primitives.h"

///Class Entity ---------------------------------
//Constructors ========================
Entity::Entity(const std::string& name, const iPoint& position, ENTITY_TYPE entity_type) : name(name), position(position), entity_type(entity_type)
{

}

Entity::Entity(const Entity* copy) :position(copy->position), entity_type(copy->entity_type)
{

}

//Destructors =========================
Entity::~Entity()
{

}


//Functionality =======================
// Draw -----------
bool Entity::Draw()
{
	bool ret = false;
	//Draw Entity Mark
	ret = mark->Draw();

	//Draw Entity Current animation frame
	SDL_Rect rect = current_animation->GetCurrentFrame();
	iPoint pivot = current_animation->GetCurrentPivot();
	ret = App->render->Blit(current_animation->GetTexture(), (int)position.x - pivot.x, (int)position.y - pivot.y, &rect);

	return ret;
}
//Set Methods -----
void Entity::SetName(const char * name_str)
{
	name = name_str;
}

void Entity::SetPosition(int x, int y)
{
	position.x = x;
	position.y = y;
	mark->SetPosition(iPoint(x, y));
}

void Entity::SetEntityType(ENTITY_TYPE type)
{
	entity_type = type;
}

void Entity::SetDiplomacy(DIPLOMACY new_diplomacy)
{
	entity_diplomacy = new_diplomacy;
}

void Entity::SetAnimation(const Animation * anim)
{
	current_animation = (Animation*)anim;
}

void Entity::GenerateMark()
{
	switch (entity_type)
	{
	case UNIT:
		mark = new Circle(position, 20);
		mark->SetColor({ 250,50,40,255 });
		mark->SetXAngle(7);
		break;
	case RESOURCE:
		break;
	case BUILDING:
		mark = new Rectng(iPoint(position.x,position.y - 25), 400);
		mark->SetColor({ 150,250,40,255 });
		mark->SetXAngle(7);
		break;
	}
}
// ----------------
//Get Methods -----
const char* Entity::GetName() const
{
	return name.c_str();
}

iPoint Entity::GetPosition() const
{
	return position;
}

ENTITY_TYPE Entity::GetEntityType() const
{
	return entity_type;
}

DIPLOMACY Entity::GetDiplomacy() const
{
	return entity_diplomacy;
}

Animation * Entity::GetAnimation() const
{
	return current_animation;
}

Primitive * Entity::GetMark() const
{
	return mark;
}

// ----------------
///----------------------------------------------
///Class Unit -----------------------------------
//Constructors ========================
Unit::Unit(const std::string& name): Entity(name)
{

}

Unit::Unit(const Unit * copy) :Entity(copy->name, copy->position, copy->entity_type), unit_type(unit_type), max_life(max_life), life(life), view_area(view_area),
speed(speed), action_type(action_type), direction_type(direction_type), attack_hitpoints(attack_hitpoints), attack_bonus(attack_bonus), siege_hitpoints(siege_hitpoints),
attack_rate(attack_rate), attack_type(attack_type), attack_range(attack_range), defence(defence), defence_bonus(defence_bonus), armor(armor), armor_bonus(armor_bonus),
food_cost(food_cost), wood_cost(wood_cost), coin_cost(coin_cost), poblation_cost(poblation_cost), train_time(train_time)
{

}

//Destructors =========================
Unit::~Unit()
{

}

//Functionality =======================
//Set Methods -----
void Unit::SetUnitType(UNIT_TYPE type)
{
	unit_type = type;
}

void Unit::SetFullLife(uint full_life_val)
{
	max_life = full_life_val;
}

void Unit::SetLife(uint life_val)
{
	life = life_val;
}

void Unit::SetViewArea(uint area_val)
{
	view_area = area_val;
}

void Unit::SetSpeed(float speed_val)
{
	speed = speed_val;
}

void Unit::SetAction(ACTION_TYPE action_val)
{
	action_type = action_val;
}

void Unit::SetDirection(DIRECTION_TYPE direction_val)
{
	direction_type = direction_val;
}

void Unit::SetAttackHitPoints(uint atk_val)
{
	attack_hitpoints = atk_val;
}

void Unit::SetAttackBonus(uint atk_bonus)
{
	attack_bonus = atk_bonus;
}

void Unit::SetSiegeHitPoints(uint siege_val)
{
	siege_hitpoints = siege_val;
}

void Unit::SetAttackRate(float atk_rate)
{
	attack_rate = atk_rate;
}

void Unit::SetAttackType(ATTACK_TYPE atk_type)
{
	attack_type = atk_type;
}

void Unit::SetAttackRange(float atk_range)
{
	attack_range = atk_range;
}

void Unit::SetDefence(uint def)
{
	defence = def;
}

void Unit::SetDefenceBonus(uint def_bonus)
{
	defence_bonus = def_bonus;
}

void Unit::SetArmor(uint arm)
{
	armor = arm;
}

void Unit::SetArmorBonus(uint arm_bonus)
{
	armor_bonus = arm_bonus;
}

void Unit::SetFoodCost(uint food_cst)
{
	food_cost = food_cst;
}

void Unit::SetWoodCost(uint wood_cst)
{
	wood_cost = wood_cst;
}

void Unit::SetCoinCost(uint coin_cst)
{
	coin_cost = coin_cst;
}

void Unit::SetPoblationCost(uint poblation_cst)
{
	poblation_cost = poblation_cst;
}

void Unit::SetTrainTime(uint train_time_val)
{
	train_time = train_time_val;
}
void Unit::SetExp(uint experience)
{
	exp = experience;
}
// ----------------
//Get Methods -----
UNIT_TYPE Unit::GetUnitType()const
{
	return unit_type;
}

uint Unit::GetFullLife() const
{
	return max_life;
}

uint Unit::GetLife() const
{
	return life;
}

uint Unit::GetViewArea()const
{
	return view_area;
}

float Unit::GetSpeed()const
{
	return speed;
}

ACTION_TYPE Unit::GetAction()const
{
	return action_type;
}

DIRECTION_TYPE Unit::GetDirection()const
{
	return direction_type;
}

uint Unit::GetAttackHitPoints()const
{
	return attack_hitpoints;
}

uint Unit::GetAttackBonus()const
{
	return attack_bonus;
}

uint Unit::GetSiegeHitPoints()const
{
	return siege_hitpoints;
}

float Unit::GetAttackRate()const
{
	return attack_rate;
}

ATTACK_TYPE Unit::GetAttackType()const
{
	return attack_type;
}

float Unit::GetAttackRange()const
{
	return attack_range;
}

uint Unit::GetDefence()const
{
	return defence;
}

uint Unit::GetDefenceBonus() const
{
	return defence_bonus;
}

uint Unit::GetArmor() const
{
	return armor;
}

uint Unit::GetArmorBonus() const
{
	return armor_bonus;
}

uint Unit::GetFoodCost() const
{
	return food_cost;
}

uint Unit::GetWoodCost() const
{
	return wood_cost;
}

uint Unit::GetCoinCost() const
{
	return coin_cost;
}

uint Unit::GetPoblationCost() const
{
	return poblation_cost;
}

uint Unit::GetTrainTime() const
{
	return train_time;
}
uint Unit::GetExp() const
{
	return exp;
}
// ----------------
///----------------------------------------------

///Class Resource -------------------------------
//Constructors ========================
Resource::Resource(const std::string & name):Entity(name)
{

}

Resource::Resource(const Resource * copy) : Entity(copy->name, copy->position, copy->entity_type), resource_type(resource_type), max_resources(max_resources), current_resources(current_resources)
{

}

//Destructors =========================
Resource::~Resource()
{
}

//Functionality =======================
void Resource::SetResourceType(RESOURCE_TYPE type)
{
	resource_type = type;
}

void Resource::SetMaxResources(uint max_res)
{
	max_resources = max_res;
}

void Resource::SetCurrentResources(uint current_res)
{
	current_resources = current_res;
}

RESOURCE_TYPE Resource::GetResourceType() const
{
	return resource_type;
}

uint Resource::GetMaxResources() const
{
	return max_resources;
}

uint Resource::GetCurrentResources() const
{
	return current_resources;
}
///----------------------------------------------

///Class Building -------------------------------
//Constructors ========================
Building::Building(const std::string & name):Entity(name)
{

}

Building::Building(const Building* copy) : Entity(copy->name, copy->position, copy->entity_type), buiding_type(copy->buiding_type), max_life(copy->max_life), life(copy->life), units_capacity(units_capacity), current_units(copy->current_units)
{

}

//Destructors =========================
Building::~Building()
{
	std::list<Unit*>::iterator item = units_in.begin();
	while (item != units_in.end())
	{
		RELEASE(item._Ptr->_Myval);
		item++;
	}
	units_in.clear();
}

//Functionality =======================
//Units Generator -----------
Unit * Building::GenerateUnit(UNIT_TYPE new_unit_type) const
{
	return nullptr;
}

//Draw ----------------------
bool Building::Draw()
{
	bool ret = false;
	ret = mark->Draw();
	const std::vector<SDL_Rect>*	sprites = current_animation->GetAllFrames();
	const std::vector<iPoint>*		pivots = current_animation->GetAllPivots();

	uint size = sprites->size();
	for (uint k = 0; k < size; k++)
	{
		if (!App->render->Blit(current_animation->GetTexture(), (int)position.x - pivots->at(k).x, (int)position.y - pivots->at(k).y, &sprites->at(k)))
		{
			ret = false;
			break;
		}
	}

	return ret;
}

//Set Methods ---------------
void Building::SetBuildingType(BUILDING_TYPE type)
{
	buiding_type = type;
}

void Building::SetMaxLife(uint max_life_val)
{
	max_life = max_life_val;
}

void Building::SetLife(uint life_val)
{
	life = life_val;
}

void Building::SetUnitsCapacity(uint capacity)
{
	units_capacity = capacity;
}

void Building::SetCurrentUnits(uint units)
{
	current_units = units;
}

//Get Methods ---------------
BUILDING_TYPE Building::GetBuildingType() const
{
	return buiding_type;
}

uint Building::GetMaxLife() const
{
	return max_life;
}

uint Building::GetLife() const
{
	return life;
}

uint Building::GetUnitsCapacity() const
{
	return units_capacity;
}

uint Building::GetCurrentUnits() const
{
	return current_units;
}
///----------------------------------------------
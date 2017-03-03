#include "BaseEntities.h"

#include "j1App.h"
#include "j1Render.h"
#include "j1Animator.h"


///Class Entity ---------------------------------
//Constructors ========================
Entity::Entity() :name("")
{

}

Entity::Entity(const std::string& name, const fPoint& position, ENTITY_TYPE entity_type) : name(name), position(position), entity_type(entity_type)
{

}

Entity::Entity(const Entity& copy) : name(copy.name), position(copy.position), entity_type(copy.entity_type), entity_diplomacy(copy.entity_diplomacy), icon_rect(copy.icon_rect)
{

}

//Destructors =========================
Entity::~Entity()
{

}


//Functionality =======================
//Select/Deselect ------
void Entity::Select()
{
	selected = true;
}

void Entity::Deselect()
{
	selected = false;
}

// Draw -----------
bool Entity::Draw(bool debug)
{
	bool ret = false;

	//Draw Entity Current animation frame
	const Sprite* sprite = current_animation->GetCurrentSprite();
	ret = App->render->CallBlit(current_animation->GetTexture(), position.x, position.y, sprite->GetFrame(), flip_sprite, -position.y - sprite->GetZ_cord(), sprite->GetXpivot(), sprite->GetYpivot());

	return ret;
}
//Set Methods -----
void Entity::SetName(const char * name_str)
{
	name = name_str;
}

void Entity::SetPosition(float x, float y)
{
	position.x = x;
	position.y = y;
}

void Entity::SetEntityType(ENTITY_TYPE type)
{
	entity_type = type;
}

void Entity::SetDiplomacy(DIPLOMACY new_diplomacy)
{
	entity_diplomacy = new_diplomacy;
}

void Entity::SetAnimation(Animation * anim)
{
	current_animation = (Animation*)anim;
}

void Entity::SetFlipSprite(bool flip)
{
	flip_sprite = flip;
}

void Entity::SetIcon(const SDL_Rect & icon)
{
	icon_rect = icon;
}

// ----------------
//Get Methods -----
const char* Entity::GetName() const
{
	return name.c_str();
}

const fPoint& Entity::GetPosition() const
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

bool Entity::GetFlipSprite() const
{
	return flip_sprite;
}

const SDL_Rect& Entity::GetIcon()const
{
	return icon_rect;
}

// ----------------
///----------------------------------------------


///Class Unit -----------------------------------
//Constructors ========================
Unit::Unit() :Entity()
{

}

Unit::Unit(const std::string& name): Entity(name)
{

}

Unit::Unit(const Unit& copy) : Entity(copy), unit_type(copy.unit_type), selection_rect(copy.selection_rect), mark(copy.mark), max_life(copy.max_life), life(copy.life), view_area(copy.view_area),
speed(copy.speed), action_type(copy.action_type), direction_type(copy.direction_type), attack_hitpoints(copy.attack_hitpoints), attack_bonus(copy.attack_bonus), siege_hitpoints(copy.siege_hitpoints),
attack_rate(copy.attack_rate), attack_type(copy.attack_type), attack_range(copy.attack_range), defense(copy.defense), defense_bonus(copy.defense_bonus), armor(copy.armor), armor_bonus(copy.armor_bonus),
food_cost(copy.food_cost), wood_cost(copy.wood_cost), coin_cost(copy.coin_cost), population_cost(copy.population_cost), train_time(copy.train_time)
{

}

//Destructors =========================
Unit::~Unit()
{

}

//Functionality =======================
//Draw ------------
bool Unit::Draw(bool debug)
{
	bool ret = false;
	
	//Draw Entity Mark
	if(selected)ret = mark.Draw();
	
	if (debug) {
		//Draw Entity Selection Rect
		App->render->DrawQuad({ (int)floor(position.x + selection_rect.x - selection_rect.w * 0.5f),(int)position.y + selection_rect.y, selection_rect.w,-selection_rect.h }, 50, 155, 255, 100, true);

		//Draw axis lines to check the center of the unit (tool used during the sprites allocation)
		int length = 55;
		iPoint p1 = { (int)position.x, (int)position.y - length };
		iPoint p2 = { (int)position.x, (int)position.y + length };
		SDL_Color color = { 50,255,200,255 };
		Line y_axis(p1, p2, color);
		p1.x -= length;
		p1.y += length;
		p2.x += length;
		p2.y -= length;
		Line x_axis(p1, p2, color);
		y_axis.Draw();
		x_axis.Draw();
	}
	
	//Draw Entity Current animation frame
	const Sprite* sprite = current_animation->GetCurrentSprite();
	ret = App->render->CallBlit(current_animation->GetTexture(), position.x, position.y, sprite->GetFrame(), flip_sprite, -position.y - sprite->GetZ_cord(), sprite->GetXpivot(), sprite->GetYpivot());

	return ret;
}

//Set Methods -----
void Unit::SetPosition(float x, float y)
{
	//Set unit position
	position.x = x;
	position.y = y;
	//Set unit mark position
	mark.SetPosition(iPoint(x, y));
}

void Unit::SetUnitType(UNIT_TYPE type)
{
	unit_type = type;
}

void Unit::SetSelectionRect(const SDL_Rect & rect)
{
	selection_rect = rect;
}

void Unit::SetMark(const Circle & new_mark)
{
	mark = new_mark;
}

void Unit::SetMaxLife(uint full_life_val)
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

void Unit::SetAttackRate(uint atk_rate)
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

void Unit::SetDefense(uint def)
{
	defense = def;
}

void Unit::SetDefenseBonus(uint def_bonus)
{
	defense_bonus = def_bonus;
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

void Unit::SetPopulationCost(uint poblation_cst)
{
	population_cost = poblation_cst;
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

const SDL_Rect * Unit::GetSelectionRect() const
{
	return &selection_rect;
}

const Circle& Unit::GetMark() const
{
	return mark;
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

uint Unit::GetDefense()const
{
	return defense;
}

uint Unit::GetDefenseBonus() const
{
	return defense_bonus;
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

uint Unit::GetPopulationCost() const
{
	return population_cost;
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

Resource::Resource(const Resource& copy) : Entity(copy), resource_type(resource_type), max_resources(max_resources), current_resources(current_resources)
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

Building::Building(const Building& copy) : Entity(copy), mark(copy.mark), buiding_type(copy.buiding_type), max_life(copy.max_life), life(copy.life), units_capacity(units_capacity), current_units(copy.current_units)
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
Unit * Building::CraftUnit(UNIT_TYPE new_unit_type) const
{
	return nullptr;
}

//Draw ----------------------
bool Building::Draw(bool debug)
{
	bool ret = false;

	//Debug Draw
	//if(debug)ret = mark->Draw();

	//Get all sprites of the current animation
	const std::vector<Sprite>*	sprites = current_animation->GetAllSprites();

	uint size = sprites->size();
	for (uint k = 0; k < size; k++)
	{
		ret = App->render->CallBlit(current_animation->GetTexture(), position.x - sprites->at(k).GetXpivot(), position.y - sprites->at(k).GetYpivot(), sprites->at(k).GetFrame(), false, -position.y - sprites->at(k).GetZ_cord());
		if(!ret)break;
	}

	return ret;
}

void Building::SetMark(const Rectng & rectangle)
{
	mark = rectangle;
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

const Rectng & Building::GetMark() const
{
	return mark;
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
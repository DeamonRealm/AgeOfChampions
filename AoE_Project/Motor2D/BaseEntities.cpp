#include "BaseEntities.h"

#include "j1App.h"
#include "j1Render.h"
#include "j1Animator.h"
#include "j1Pathfinding.h"
#include "j1Fonts.h"
#include "j1Map.h"
#include "p2Log.h"
#include "j1EntitiesManager.h"

///Class Entity ---------------------------------
//Constructors ========================
Entity::Entity() :name("")
{

}

Entity::Entity(const Entity& copy) : name(copy.name), position(copy.position), entity_type(copy.entity_type), entity_diplomacy(copy.entity_diplomacy), selection_rect(copy.selection_rect),
icon_rect(copy.icon_rect), max_life(copy.max_life), life(copy.life), current_animation(copy.current_animation)
{

}

//Destructors =========================
Entity::~Entity()
{

}


//Functionality =======================
//Select/Deselect -
void Entity::Select()
{
	selected = true;
}

void Entity::Deselect()
{
	selected = false;
}

//Update ----------
bool Entity::Update()
{
	return true;
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

void Entity::SetMaxLife(uint full_life_val)
{
	max_life = full_life_val;
}

void Entity::SetLife(uint life_val)
{
	life = life_val;
}

void Entity::SetAnimation(Animation * anim)
{
	current_animation = (Animation*)anim;
}

void Entity::SetFlipSprite(bool flip)
{
	flip_sprite = flip;
}

void Entity::SetSelectionRect(const SDL_Rect & rect)
{
	selection_rect = rect;
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

iPoint Entity::GetPositionRounded() const
{
	return iPoint(position.x,position.y);
}

ENTITY_TYPE Entity::GetEntityType() const
{
	return entity_type;
}

DIPLOMACY Entity::GetDiplomacy() const
{
	return entity_diplomacy;
}

uint Entity::GetMaxLife() const
{
	return max_life;
}

uint Entity::GetLife() const
{
	return life;
}

Animation * Entity::GetAnimation() const
{
	return current_animation;
}

bool Entity::GetFlipSprite() const
{
	return flip_sprite;
}

const SDL_Rect * Entity::GetSelectionRect() const
{
	return &selection_rect;
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

Unit::Unit(const Unit& copy) : Entity(copy), unit_type(copy.unit_type), mark(copy.mark), view_area(copy.view_area),
speed(copy.speed), action_type(copy.action_type), direction_type(copy.direction_type), attack_hitpoints(copy.attack_hitpoints), attack_bonus(copy.attack_bonus), siege_hitpoints(copy.siege_hitpoints),
attack_rate(copy.attack_rate), attack_type(copy.attack_type), attack_range(copy.attack_range), defense(copy.defense), defense_bonus(copy.defense_bonus), armor(copy.armor), armor_bonus(copy.armor_bonus),
food_cost(copy.food_cost), wood_cost(copy.wood_cost), gold_cost(copy.gold_cost), population_cost(copy.population_cost), train_time(copy.train_time)
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
		if (path != nullptr)DrawPath();
	
	}
	
	//Draw Entity Current animation frame
	const Sprite* sprite = current_animation->GetCurrentSprite();
	ret = App->render->CallBlit(current_animation->GetTexture(), position.x, position.y, sprite->GetFrame(), flip_sprite, -position.y - sprite->GetZ_cord(),sprite->GetOpacity(), sprite->GetXpivot(), sprite->GetYpivot());

	return ret;
}

bool Unit::DrawPath()
{
	if (path == nullptr) return false;

	uint size = path->size();
	for (uint k = 0; k < size; k++)
	{
		iPoint cell = path->at(k);
		App->render->CallBlit(App->pathfinding->path_texture, cell.x - App->map->data.tile_width * 0.5f, cell.y - App->map->data.tile_height * 0.5);
	}
	
	return true;
}

//Update ----------
bool Unit::Update()
{
	if (path != nullptr)return Move();
	if (interaction_target != nullptr)Interact();
	return false;
}

//Actions ---------
bool Unit::Move()
{
	//Check if the unit have an assigned path
	if (path == nullptr)
	{
		LOG("Error path not found!");
		return false;
	}

	//Build location & goal path points
	iPoint location = iPoint(position.x, position.y);
	iPoint goal = path->back();

	//Update goal node and animation direction
	if ((iPoint(position.x, position.y) - goal) < iPoint(1, 1))
	{
		if (path->size() == 1)
		{
			//Set unit at the goal pixel position
			position.x = (float)goal.x;
			position.y = (float)goal.y;

			//Stop idle walk animation
			action_type = IDLE;
			App->animator->UnitPlay(this);
			
			//Delete unit path
			delete path;
			path = nullptr;

			if (interaction_target != nullptr)Interact();
			LOG("Path completed!");
			return true;
		}

		//Set the unit next tile goal
		path->pop_back();
		goal = path->back();
		//Focus the unit at the next goal
		Focus(goal);
	}

	//Calculate the X/Y values that the unit have to move 
	//checking the goal location and the unit movement speed
	int norm = location.DistanceTo(goal);
	float x_step = speed * (goal.x - location.x) / norm;
	float y_step = speed * (goal.y - location.y) / norm;

	//Add the calculated values at the unit & mark position
	position.x += x_step;
	position.y += y_step;
	mark.SetPosition(iPoint(position.x,position.y));

	return true;
}

bool Unit::Interact()
{
	//Calculate the distance between the unit and the resource 
	double distance = sqrt(abs(interaction_target->GetPositionRounded().DistanceNoSqrt(iPoint(position.x, position.y))));
	//Check if the resource is in the action area of the villager
	if (view_area < distance)
	{
		App->entities_manager->SetUnitPath(this, interaction_target->GetPositionRounded());
		return false;
	}

	//If the target is in the interaction area the unit do the correct action with it
	switch (interaction_target->GetEntityType())
	{
	case UNIT:			if(action_timer.Read() > attack_rate)Attack();		break;
	case BUILDING:		Cover();											break;
	}
	return true;
}

void Unit::Focus(const iPoint & target)
{
	//Calculate the directional vector
	iPoint dir_point = target - iPoint(position.x, position.y);

	//Find the correct direction in relation of the goal and the location
	if (abs(dir_point.x) < 4)
	{
		if (dir_point.y > 0)direction_type = DIRECTION_TYPE::SOUTH;
		else direction_type = DIRECTION_TYPE::NORTH;
	}
	else if (abs(dir_point.y) < 4)
	{
		if (dir_point.x > 0)direction_type = DIRECTION_TYPE::EAST;
		else direction_type = DIRECTION_TYPE::WEST;
	}
	else if (dir_point.x >= 0 && dir_point.y >= 0)
	{
		direction_type = DIRECTION_TYPE::SOUTH_EAST;
	}
	else if (dir_point.x <= 0 && dir_point.y >= 0)
	{
		direction_type = DIRECTION_TYPE::SOUTH_WEST;
	}
	else if (dir_point.x >= 0 && dir_point.y <= 0)
	{
		direction_type = DIRECTION_TYPE::NORTH_EAST;
	}
	else if (dir_point.x <= 0 && dir_point.y <= 0)
	{
		direction_type = DIRECTION_TYPE::NORTH_WEST;
	}
	
	//Set the unit animation with the new direction
	App->animator->UnitPlay(this);
}

bool Unit::Attack()
{
	if (interaction_target->GetLife() == 0)
	{
		App->entities_manager->DeleteEntity(interaction_target);
		return false;
	}
	//Calculate the attack & apply the value at the target life points
	((Unit*)interaction_target)->life -= MIN(((Unit*)interaction_target)->life, attack_hitpoints);

	//Reset action timer
	action_timer.Start();
	return false;
}

bool Unit::Cover()
{
	return ((Building*)interaction_target)->CoverUnit(this);;
}

//Bonus -----------
void Unit::AddBonus(BONUS_TYPE type, uint type_id, uint bonus, bool defence)
{
	Bonus* new_bonus = new Bonus(type, type_id, bonus);
	if (defence) defence_bonuses.push_back(new_bonus);
	else defence_bonuses.push_back(new_bonus);
}

//Set Methods -----
void Unit::SetPosition(float x, float y)
{
	//Set unit position
	position.x = x;
	position.y = y;
	//Set unit mark position
	mark.SetPosition(iPoint( x, y ));
}

void Unit::SetUnitType(UNIT_TYPE type)
{
	unit_type = type;
}

void Unit::SetInteractionTarget(const Entity * target)
{
	interaction_target = (Entity*)target;
}

void Unit::SetMark(const Circle & new_mark)
{
	mark = new_mark;
}

void Unit::SetViewArea(float area_val)
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

void Unit::SetAttackDelay(uint atk_delay)
{
	attack_delay = atk_delay;
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

void Unit::SetGoldCost(uint coin_cst)
{
	gold_cost = coin_cst;
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
void Unit::SetPath(const std::vector<iPoint>* new_path)
{
	path = (std::vector<iPoint>*)new_path;
}
// ----------------
//Get Methods -----
UNIT_TYPE Unit::GetUnitType()const
{
	return unit_type;
}

const Circle& Unit::GetMark() const
{
	return mark;
}

const Entity * Unit::GetInteractionTarget()
{
	return interaction_target;
}

float Unit::GetViewArea()const
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

uint Unit::GetAttackDelay() const
{
	return attack_delay;
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

uint Unit::GetAttackRate()const
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

uint Unit::GetGoldCost() const
{
	return gold_cost;
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
Resource::Resource() :Entity()
{

}

Resource::Resource(const Resource& copy) : Entity(copy), resource_type(copy.resource_type), mark(copy.mark), max_resources(copy.max_resources), current_resources(copy.current_resources)
{

}

//Destructors =========================
Resource::~Resource()
{

}

//Functionality =======================
bool Resource::Draw(bool debug)
{
	bool ret = false;
	//Draw Resource Mark
	ret = mark.Draw();

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

	const std::vector<Sprite>* sprites = current_animation->GetAllSprites();

	uint size = sprites->size();
	for (uint k = 0; k < size; k++)
	{
		ret = App->render->CallBlit(current_animation->GetTexture(), position.x - sprites->at(k).GetXpivot(), position.y - sprites->at(k).GetYpivot(), sprites->at(k).GetFrame(), false, -position.y - sprites->at(k).GetZ_cord(), sprites->at(k).GetOpacity());
		if (!ret)break;
	}

	return ret;
}

bool Resource::ExtractResources(uint* value)
{
	if (life == 0)return false;
	if (life <= *value)
	{
		*value = life;
		life = 0;
		App->entities_manager->DeleteEntity(this);
		return true;
	}
	else
	{
		life -= *value;
		if (resource_type == GOLD_ORE && life < 50)
		{
			resource_type = TINY_GOLD_ORE;
			App->animator->ResourcePlay(this);
		}
		else if (resource_type == STONE_ORE && life < 50)
		{
			resource_type = TINY_STONE_ORE;
			App->animator->ResourcePlay(this);
		}

		return true;
	}
}

void Resource::SetPosition(float x, float y)
{
	//Set building position fixing it in the tiles coordinates
	iPoint coords = App->map->WorldToMap(x, y);
	coords = App->map->MapToWorld(coords.x, coords.y);
	position.x = coords.x;
	position.y = coords.y;

	//Set building mark position
	mark.SetPosition(iPoint(position.x, position.y));
}

void Resource::SetMark(const Rectng & rectangle)
{
	mark = rectangle;
}

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

const Rectng& Resource::GetMark() const
{
	return mark;
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
Building::Building() :Entity()
{

}

Building::Building(const Building& copy) : Entity(copy), mark(copy.mark), building_type(copy.building_type), max_life(copy.max_life), life(copy.life), units_capacity(units_capacity), current_units(copy.current_units)
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
	//Call entities manager to generate a new unit
	Unit* new_unit = App->entities_manager->GenerateUnit(new_unit_type, false);
	//Set the new unit position at the buiding spawn point
	new_unit->SetPosition(position.x + (float)units_spawn_point.x, position.y + (float)units_spawn_point.y);

	//Add the unit in the crafting units priority queue
	//production_queue.emplace(new_unit);

	
	return new_unit;
}

bool Building::CoverUnit(const Unit * target)
{
	if (units_capacity == current_units)
	{
		LOG("Building is full!");
		return false;
	}

	units_in.push_back(App->entities_manager->PopUnit(target));

	return true;
}

void Building::ReleaseUnit(const Unit * target)
{
	units_in.remove((Unit*)target);
	App->entities_manager->AddUnit(target);
}

void Building::ReleaseAllUnits()
{
	uint size = units_in.size();
	for (uint k = 0; k < size; k++)
	{
		if (units_in.back()->GetUnitType() == VILLAGER)units_in.back()->Interact();
		App->entities_manager->AddUnit(units_in.back());
		units_in.pop_back();
	}
}

//Draw ----------------------
bool Building::Draw(bool debug)
{
	bool ret = false;

	//Debug Draw
	ret = mark.Draw();

	if (debug) {
		//Draw Entity Selection Rect
		App->render->DrawQuad({ (int)floor(position.x + selection_rect.x - selection_rect.w * 0.5f),(int)position.y + selection_rect.y, selection_rect.w,-selection_rect.h }, 50, 155, 255, 100, true);

		//Draw axis lines to check the center of the unit (tool used during the sprites allocation)
		int length = 200;
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

	//Get all sprites of the current animation
	const std::vector<Sprite>*	sprites = current_animation->GetAllSprites();

	uint size = sprites->size();
	for (uint k = 0; k < size; k++)
	{
		ret = App->render->CallBlit(current_animation->GetTexture(), position.x - sprites->at(k).GetXpivot(), position.y - sprites->at(k).GetYpivot(), sprites->at(k).GetFrame(), false, -position.y - sprites->at(k).GetZ_cord(),sprites->at(k).GetOpacity());
		if(!ret)break;
	}

	return ret;
}

void Building::SetPosition(float x, float y)
{
	//Set building position fixing it in the tiles coordinates
	iPoint coords = App->map->WorldToMap(x, y);
	coords = App->map->MapToWorld(coords.x, coords.y);
	position.x = coords.x;
	position.y = coords.y - App->map->data.tile_height * 0.5f;

	//Set building mark position
	mark.SetPosition(iPoint(position.x, position.y));
}

void Building::SetMark(const Rectng& rectangle)
{
	mark = rectangle;
}

//Set Methods ---------------
void Building::SetBuildingType(BUILDING_TYPE type)
{
	building_type = type;
}

void Building::SetActionType(ACTION_TYPE type)
{
	action_type = type;
}

void Building::SetDirectionType(DIRECTION_TYPE type)
{
	direction_type = type;
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

void Building::SetUnitsSpawnPoint(const iPoint & point)
{
	units_spawn_point = point;
}

const Rectng & Building::GetMark() const
{
	return mark;
}

//Get Methods ---------------
BUILDING_TYPE Building::GetBuildingType() const
{
	return building_type;
}

ACTION_TYPE Building::GetActionType() const
{
	return action_type;
}

DIRECTION_TYPE Building::GetDirectionType() const
{
	return direction_type;
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
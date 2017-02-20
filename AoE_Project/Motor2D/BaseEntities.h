#ifndef _BASE_ENTITIES_
#define _BASE_ENTITIES_

#include <list>
#include "p2Defs.h"
#include "p2Point.h"

struct Animation;
struct SDL_Texture;

// ENTITIES ENUMS -----------
enum ENTITY_TYPE
{
	NO_ENTITY = 0,
	UNIT,
	RESOURCE,
	BUILDING
};
enum UNIT_TYPE
{
	NO_UNIT = 0,
	MILITIA,
	ARBALEST
};
enum RESOURCE_TYPE
{
	NO_RESOURCE = 0,
	TREE
};
enum BUILDING_TYPE
{
	NO_BUILDING = 0,
	FARM,
	TOWN_CENTER
};
enum ACTION_TYPE
{
	NO_ACTION = 0,
	ATTATCK,
	DIE,
	DISAPEAR,
	IDLE,
	WALK
};
enum ATTACK_TYPE
{
	NO_ATTACK = 0,
	MELEE,
	DISTANCE,
};
enum DIRECTION_TYPE
{
	NO_DIRECTION = 0,
	NORTH,
	NORTH_EAST,
	EAST,
	SOUTH_EAST,
	SOUTH,
	SOUTH_WEST,
	WEST,
	NORTH_WEST
};
// --------------------------

///Class Entity ---------------------------------
//Base Class that define the general attributes of all kind of entities
class Entity
{
public:

	Entity(const std::string& name, const  fPoint& position = { 0,0 } , ENTITY_TYPE entity_type = NO_ENTITY);
	Entity(const Entity* copy);

	~Entity();

protected:

	std::string		name;
	fPoint			position = { 0,0 };
	ENTITY_TYPE		entity_type = NO_ENTITY;
	SDL_Texture*	texture = nullptr;
	Animation*		current_animation = nullptr;

public:



	//Functionality -------------------
	//Draw ------------------
	virtual bool	Draw();

	//Set Methods -----------
	void			SetName(const char* name_str);
	void			SetPosition(float x, float y);
	void			SetEntityType(ENTITY_TYPE type);
	void			SetTexture(SDL_Texture* tex);
	void			SetAnimation(const Animation* anim);

	//Get Methods -----------
	const char*		GetName()const;
	fPoint			GetPosition()const;
	ENTITY_TYPE		GetEntityType()const;
	SDL_Texture*	GetTexture()const;
	Animation*		GetAnimation()const;
};
/// ---------------------------------------------

///Class Unit -----------------------------------
//Base class that define the general attributes for all units
class Unit : public Entity
{
public:

	Unit(const std::string& name);
	Unit(const Unit* copy);

	~Unit();

protected:

	//Stats ----------------------
	UNIT_TYPE		unit_type = NO_UNIT;
	//Life -------------
	uint			max_life = 0;
	float			life = 0;
	//Movement ---------
	uint			view_area = 0;
	float			speed = 0;
	ACTION_TYPE		action_type = NO_ACTION;
	DIRECTION_TYPE	direction_type = NO_DIRECTION;
	//Attack -----------
	uint			attack_hitpoints = 0;
	uint			attack_bonus = 0;
	uint			siege_hitpoints = 0;
	float			attack_rate = 0;
	ATTACK_TYPE		attack_type = NO_ATTACK;
	float			attack_range = 0;
	//Defence/Armor ----
	uint			defence = 0;
	uint			defence_bonus = 0;
	uint			armor = 0;
	uint			armor_bonus = 0;
	//Resources --------
	uint			food_cost = 0;
	uint			wood_cost = 0;
	uint			coin_cost = 0;
	uint			poblation_cost = 0;
	uint			train_time = 0;
	uint			exp = 0;

public:

	//Functionality -------------------
	//Set Methods -----------
	void	SetUnitType(UNIT_TYPE type);
	void	SetFullLife(uint full_life_val);
	void	SetLife(uint life_val);
	void	SetViewArea(uint area_val);
	void	SetSpeed(float speed_val);
	void	SetAction(ACTION_TYPE action_val);
	void	SetDirection(DIRECTION_TYPE direction_val);
	void	SetAttackHitPoints(uint atk_val);
	void	SetAttackBonus(uint atk_bonus);
	void	SetSiegeHitPoints(uint siege_val);
	void	SetAttackRate(float atk_rate);
	void	SetAttackType(ATTACK_TYPE atk_type);
	void	SetAttackRange(float atk_range);
	void	SetDefence(uint def);
	void	SetDefenceBonus(uint def_bonus);
	void	SetArmor(uint arm);
	void	SetArmorBonus(uint arm_bonus);
	void	SetFoodCost(uint food_cst);
	void	SetWoodCost(uint wood_cst);
	void	SetCoinCost(uint coin_cst);
	void	SetPoblationCost(uint poblation_cst);
	void	SetTrainTime(uint train_time_val);
	void	SetExp(uint experience);

	//Get Methods -----------
	UNIT_TYPE		GetUnitType()const;
	uint			GetFullLife()const;
	uint			GetLife()const;
	uint			GetViewArea()const;
	float			GetSpeed()const;
	ACTION_TYPE		GetAction()const;
	DIRECTION_TYPE	GetDirection()const;
	uint			GetAttackHitPoints()const;
	uint			GetAttackBonus()const;
	uint			GetSiegeHitPoints()const;
	float			GetAttackRate()const;
	ATTACK_TYPE		GetAttackType()const;
	float			GetAttackRange()const;
	uint			GetDefence()const;
	uint			GetDefenceBonus()const;
	uint			GetArmor()const;
	uint			GetArmorBonus()const;
	uint			GetFoodCost()const;
	uint			GetWoodCost()const;
	uint			GetCoinCost()const;
	uint			GetPoblationCost()const;
	uint			GetTrainTime()const;
	uint			GetExp()const;
};
/// ---------------------------------------------

///Class Resource -------------------------------
//Base class that defines standard attributes of resources
class Resource : public Entity
{
public:

	Resource(const std::string& name);
	Resource(const Resource* copy);
	~Resource();

protected:

	RESOURCE_TYPE	resource_type = NO_RESOURCE;
	uint			max_resources = 0;
	uint			current_resources = 0;

public:

	//Functionality -------------------
	//Set Methods -----------
	void	SetResourceType(RESOURCE_TYPE type);
	void	SetMaxResources(uint max_res);
	void	SetCurrentResources(uint current_res);

	//Get Methods -----------
	RESOURCE_TYPE	GetResourceType()const;
	uint			GetMaxResources()const;
	uint			GetCurrentResources()const;

};
/// ---------------------------------------------

///Class Building -------------------------------
class Building : public Entity
{
public:

	Building(const std::string& name);
	Building(const Building* copy);
	~Building();

protected:

	BUILDING_TYPE	buiding_type = NO_BUILDING;
	uint			max_life = 0;
	uint			life = 0;
	uint			units_capacity = 0;
	uint			current_units = 0;

	//List of the units placed inside the buiding
	std::list<Unit*>	units_in;

public:

	//Functionality -------------------
	//Factory that generates any type of unit suported by the building
	virtual Unit* GenerateUnit(UNIT_TYPE new_unit_type)const;

	//Draw ------------------
	bool	Draw();

	//Set Methods -----------
	void	SetBuildingType(BUILDING_TYPE type);
	void	SetMaxLife(uint max_life_val);
	void	SetLife(uint life_val);
	void	SetUnitsCapacity(uint capacity);
	void	SetCurrentUnits(uint units);

	//Get Methods -----------
	BUILDING_TYPE	GetBuildingType()const;
	uint			GetMaxLife()const;
	uint			GetLife()const;
	uint			GetUnitsCapacity()const;
	uint			GetCurrentUnits()const;
};
/// ---------------------------------------------
#endif // _ENTITY_

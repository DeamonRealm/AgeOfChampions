#ifndef _BASE_ENTITIES_
#define _BASE_ENTITIES_

#include <list>
#include "p2Defs.h"
#include "p2Point.h"
#include "Iso_Primitives.h"
#include"SDL/include/SDL_rect.h"

struct Animation;
struct SDL_Texture;
enum CURSOR_TARGET;

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
	ARBALEST,
	ARCHER,
	CAVALIER,
	CAVALRY_ARCHER,
	ELITE_SKIRMISHER,
	HEAVY_CAVALRY_ARCHER,
	KNIGHT,
	MONK,
	PALADIN,
	PIKEMAN,
	SPEARMAN,
	//Villagers
	VILLAGER,
	VILLAGER_PICK,
	VILLAGER_PLOW,
	VILLAGER_BOW,
	VILLAGER_BASKET,
	VILLAGER_AXE,
	VILLAGER_HAMMER,
	VILLAGER_CARRY
};
enum RESOURCE_TYPE
{
	NO_RESOURCE = 0,
	TREE,
	TREE_CUT,
	CHOP,
	BERRY_BUSH,
	GOLD_ORE,
	STONE_ORE
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
	DISAPPEAR,
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

enum DIPLOMACY
{
	NEUTRAL,
	ALLY,
	ENEMY
};
// --------------------------

///Class Entity ---------------------------------
//Base Class that define the general attributes of all kind of entities
class Entity
{
public:

	Entity();
	Entity(const std::string& name, const  fPoint& position = { 0,0 } , ENTITY_TYPE entity_type = NO_ENTITY);
	Entity(const Entity& copy);

	~Entity();

protected:

	bool			selected = false;
	std::string		name;
	fPoint			position = { 0,0 };
	ENTITY_TYPE		entity_type = NO_ENTITY;
	DIPLOMACY		entity_diplomacy = NEUTRAL;
	Animation*		current_animation = nullptr;
	bool			flip_sprite = false;
	SDL_Rect		selection_rect = { 0,0,0,0 };
	SDL_Rect		icon_rect = { 0,0,0,0 };

public:



	//Functionality -------------------
	//Select/Deselect ------
	void			Select();
	void			Deselect();
	
	//Draw ------------------
	virtual bool	Draw(bool debug);
	
	//Set Methods -----------
	void			SetName(const char* name_str);
	virtual void	SetPosition(float x, float y);
	void			SetEntityType(ENTITY_TYPE type);
	void			SetDiplomacy(DIPLOMACY new_diplomacy);
	void			SetAnimation(Animation* anim);
	void			SetFlipSprite(bool flip);
	void			SetSelectionRect(const SDL_Rect& rect);
	void			SetIcon(const SDL_Rect& icon);

	//Get Methods -----------
	const char*		GetName()const;
	const fPoint&	GetPosition()const;
	ENTITY_TYPE		GetEntityType()const;
	DIPLOMACY		GetDiplomacy()const;
	Animation*		GetAnimation()const;
	bool			GetFlipSprite()const;
	const SDL_Rect*	GetSelectionRect()const;
	const SDL_Rect&	GetIcon()const;
};
/// ---------------------------------------------

///Class Unit -----------------------------------
//Base class that define the general attributes for all units
class Unit : public Entity
{
public:

	Unit();
	Unit(const std::string& name);
	Unit(const Unit& copy);

	~Unit();

protected:

	//Stats ----------------------
	UNIT_TYPE		unit_type = NO_UNIT;
	Circle			mark;
	//Life -------------
	uint			max_life = 0;
	float			life = 0;
	//Movement ---------
	uint			view_area = 0;
	float			speed = 0;
	ACTION_TYPE		action_type = IDLE;
	DIRECTION_TYPE	direction_type = SOUTH;
	//Attack -----------
	uint			attack_delay = 0;
	uint			attack_hitpoints = 0;
	uint			attack_bonus = 0;
	uint			siege_hitpoints = 0;
	uint			attack_rate = 0;
	ATTACK_TYPE		attack_type = NO_ATTACK;
	float			attack_range = 0;
	//Defense/Armor ----
	uint			defense = 0;
	uint			defense_bonus = 0;
	uint			armor = 0;
	uint			armor_bonus = 0;
	//Resources --------
	uint			food_cost = 0;
	uint			wood_cost = 0;
	uint			gold_cost = 0;
	uint			population_cost = 0;
	uint			train_time = 0;
	uint			exp = 0;

public:

	//Functionality -------------------
	//Draw ------------------
	bool	Draw(bool debug);

	//Set Methods -----------
	void	SetPosition(float x, float y);
	void	SetUnitType(UNIT_TYPE type);

	void	SetMark(const Circle& new_mark);
	void	SetMaxLife(uint full_life_val);
	void	SetLife(uint life_val);
	void	SetViewArea(uint area_val);
	void	SetSpeed(float speed_val);
	void	SetAction(ACTION_TYPE action_val);
	void	SetDirection(DIRECTION_TYPE direction_val);
	void	SetAttackDelay(uint atk_delay);
	void	SetAttackHitPoints(uint atk_val);
	void	SetAttackBonus(uint atk_bonus);
	void	SetSiegeHitPoints(uint siege_val);
	void	SetAttackRate(uint atk_rate);
	void	SetAttackType(ATTACK_TYPE atk_type);
	void	SetAttackRange(float atk_range);
	void	SetDefense(uint def);
	void	SetDefenseBonus(uint def_bonus);
	void	SetArmor(uint arm);
	void	SetArmorBonus(uint arm_bonus);
	void	SetFoodCost(uint food_cst);
	void	SetWoodCost(uint wood_cst);
	void	SetGoldCost(uint coin_cst);
	void	SetPopulationCost(uint poblation_cst);
	void	SetTrainTime(uint train_time_val);
	void	SetExp(uint experience);

	//Get Methods -----------
	UNIT_TYPE		GetUnitType()const;
	const Circle&	GetMark()const;
	uint			GetMaxLife()const;
	uint			GetLife()const;
	uint			GetViewArea()const;
	float			GetSpeed()const;
	ACTION_TYPE		GetAction()const;
	DIRECTION_TYPE	GetDirection()const;
	uint			GetAttackDelay()const;
	uint			GetAttackHitPoints()const;
	uint			GetAttackBonus()const;
	uint			GetSiegeHitPoints()const;
	uint			GetAttackRate()const;
	ATTACK_TYPE		GetAttackType()const;
	float			GetAttackRange()const;
	uint			GetDefense()const;
	uint			GetDefenseBonus()const;
	uint			GetArmor()const;
	uint			GetArmorBonus()const;
	uint			GetFoodCost()const;
	uint			GetWoodCost()const;
	uint			GetGoldCost()const;
	uint			GetPopulationCost()const;
	uint			GetTrainTime()const;
	uint			GetExp()const;
};
/// ---------------------------------------------

///Class Resource -------------------------------
//Base class that defines standard attributes of resources
class Resource : public Entity
{
public:

	Resource();
	Resource(const std::string& name);
	Resource(const Resource& copy);
	~Resource();

protected:

	Rectng			mark;
	RESOURCE_TYPE	resource_type = NO_RESOURCE;
	uint			max_resources = 0;
	uint			current_resources = 0;

public:

	//Functionality -------------------
	//Draw ------------------
	bool	Draw(bool debug);

	//Set Methods -----------
	void	SetPosition(float x, float y);
	void	SetMark(const Rectng& rectangle);
	void	SetResourceType(RESOURCE_TYPE type);
	void	SetMaxResources(uint max_res);
	void	SetCurrentResources(uint current_res);

	//Get Methods -----------
	const Rectng&	GetMark()const;
	RESOURCE_TYPE	GetResourceType()const;
	uint			GetMaxResources()const;
	uint			GetCurrentResources()const;

};
/// ---------------------------------------------

///Class Building -------------------------------
//Base class that defines standard attributes of buildings
class Building : public Entity
{
public:

	Building();
	Building(const std::string& name);
	Building(const Building& copy);
	~Building();

protected:

	Rectng			mark;
	BUILDING_TYPE	building_type = NO_BUILDING;
	ACTION_TYPE		action_type = IDLE;
	DIRECTION_TYPE	direction_type = NO_DIRECTION;
	uint			max_life = 0;
	uint			life = 0;
	uint			units_capacity = 0;
	uint			current_units = 0;

	//List of the units placed inside the building
	std::list<Unit*>	units_in;

public:

	//Functionality -------------------
	//Factory that generates any type of unit supported by the building
	virtual Unit* CraftUnit(UNIT_TYPE new_unit_type)const;

	//Draw ------------------
	bool	Draw(bool debug);

	//Set Methods -----------
	void	SetMark(const Rectng& rectangle);
	void	SetBuildingType(BUILDING_TYPE type);
	void	SetActionType(ACTION_TYPE type);
	void	SetDirectionType(DIRECTION_TYPE type);
	void	SetMaxLife(uint max_life_val);
	void	SetLife(uint life_val);
	void	SetUnitsCapacity(uint capacity);
	void	SetCurrentUnits(uint units);

	//Get Methods -----------
	const Rectng&	GetMark()const;
	BUILDING_TYPE	GetBuildingType()const;
	ACTION_TYPE		GetActionType()const;
	DIRECTION_TYPE	GetDirectionType()const;
	uint			GetMaxLife()const;
	uint			GetLife()const;
	uint			GetUnitsCapacity()const;
	uint			GetCurrentUnits()const;
};
/// ---------------------------------------------
#endif // _ENTITY_

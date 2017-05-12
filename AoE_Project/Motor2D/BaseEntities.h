#ifndef _BASE_ENTITIES_
#define _BASE_ENTITIES_

#include <list>
#include <vector>
#include <queue>
#include "p2Defs.h"
#include "p2Point.h"
#include "Iso_Primitives.h"
#include "SDL/include/SDL_rect.h"
#include "j1Timer.h"
#include "j1Animator.h"
#include "j1ActionManager.h"

struct	SDL_Texture;
struct	PathNode;
class	Warrior;
enum	CURSOR_TARGET;
class AlphaCell;
class FogTile;

#define FOCUS_MARGIN 12
#define MAX_DISTANCE 75

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
	VILLAGER,
	TWO_HANDED_SWORDMAN,
	MAN_AT_ARMS,
	LONG_SWORDMAN,
	CHAMPION,
	WARRIOR_CHMP,
	ARCHER_CHMP,
	WIZARD_CHMP,
	CROSSBOWMAN,
	SKIRMISHER
};
enum UNIT_CLASS
{
	NO_CLASS = 0,
	CIVILIAN,
	INFANTRY,
	ARCHERY,
	CAVALRY
};
enum RESOURCE_TYPE
{
	NO_RESOURCE = 0,
	TREE,
	TREE_CUT,
	CHOP,
	BERRY_BUSH,
	GOLD_ORE,
	TINY_GOLD_ORE,
	STONE_ORE,
	TINY_STONE_ORE
};
enum BUILDING_TYPE
{
	NO_BUILDING = 0,
	TOWN_CENTER,
	BARRACK,
	ARCHERY_RANGE,
	BLACKSMITH,
	STABLE,
	HOUSE_A,
	HOUSE_B,
	HOUSE_C,
	LUMBER_CAMP,
	MINING_CAMP,

	TOWN_CENTER_C,
	BARRACK_C,
	ARCHERY_RANGE_C,
	BLACKSMITH_C,
	STABLE_C,
	HOUSE_AI,
	HOUSE_BI,
	HOUSE_CI,
	MONASTERY,
	UNIVERSITY_C,
	UNIVERSITY_I,
	CASTLE,
	RUBBLE_TWO,
	RUBBLE_THREE,
	RUBBLE_FOUR
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
enum ITEM_TYPE
{
	NO_ITEM,
	AXE,
	BASKET,
	BOW,
	FISH,
	GOLD,
	STONE,
	WOOD,
	MEAT,
	HAMMER,
	PICK,
	PLOW
};
enum SOUND_TYPE
{
	NO_SOUND,
	SWORD_ATTACK_SOUND,
	ARROW_ATTACK_SOUND,
	BUILDING_ON_FIRE,
	BARRACK_SOUND,
	ARCHERY_SOUND,
	BLACKSMITH_SOUND,
	MONASTERY_SOUND,
	CASTLE_SOUND,
	FARM_SOUND,
	TOWER_SOUND,
	MILL_SOUND,
	STABLE_SOUND,
	UNIVERSITY_SOUND,
	MINING_CAMP_SOUND,
	LUMBER_CAMP_SOUND,
	TOWN_CENTER_SELECTED_SOUND,
	VILLAGER_CREATED_SOUND,
	VILLAGER_FINISH_BUILDING,
	INFANTRY_ARCHER_HORSEMAN_CREATED_SOUND,
	MONK_CONVERTING_SOUND,
	MONK_HEALING_SOUND,
	MONK_CREATED_SOUND,
	HORSE_DEATH_SOUND,
	HORSE_SELECTED_SOUND,
	HORSE_TASKED_SOUND,
	DEATH_SOUND,
	ATTACK_SOUND,
	CLICK_SOUND,
	MOVE_SOUND,
	LEVEL_UP_SOUND,
	WARRIOR_SKILL_LVL1_A,
	WARRIOR_SKILL_LVL1_B,
	WARRIOR_SKILL_LVL2_A,
	WARRIOR_SKILL_LVL2_B,
	WIZARD_SKILL_LVL2_A,
	WIZARD_SKILL_LVL2_B,
	WIZARD_SKILL_LVL3_A,
	WIZARD_SKILL_LVL3_B,
	HUNTER_SKILL_LVL2_A,
	HUNTER_SKILL_LVL2_B,
	HUNTER_SKILL_LVL3_A,
	HUNTER_SKILL_LVL3_B,
	MAIN_MENU_SONG,
	INGAME_SONG,
	WIN_SONG,
	LOST_SONG,
	CLICK_MAIN_MENU,
	CLICK_SUBMENU,
	CLICK_INGAME,
	WIN_FX,
	LOST_FX,
	ERROR_SOUND,
	PLAYER_UNIT_ALERT,
	PLAYER_BUILDING_ALERT
};

enum COLLISION_TYPE
{
	NO_COLLISION,
	COLLISION_IDLE,
	FUTURE_COLLISION_MOVE,
	COLLISION_MOVE
};
// --------------------------

///Class Arrow ----------------------------------
struct Arrow
{
	Arrow();
	~Arrow();

	//Game Loop
	bool Update();
	void Draw();

	//Data
	fPoint		position = { 0.0f,0.0f };
	fPoint		destination = { 0,0 };
	SDL_Rect	arrow_rect = { 0,0,0,0 };
	uint		pivot_x = 0;
	uint		pivot_y = 0;
	float		speed = 5.0f;
	double		angle = 0.0;
	bool		flip = false;

	//Functionality
	void PrepareArrow(fPoint start, fPoint goal);
	bool operator ==(const Arrow& other)const;
};
///----------------------------------------------

///Class Entity ---------------------------------
//Base Class that define the general attributes of all kind of entities
class Entity
{
public:

	Entity();
	Entity(const Entity& copy);

	virtual ~Entity();

protected:

	//State ------------
	bool			selected = false;
	std::string		name;
	fPoint			position = { 0,0 };
	ENTITY_TYPE		entity_type = NO_ENTITY;
	DIPLOMACY		entity_diplomacy = NEUTRAL;
	Circle			vision;
	Circle			render_area;
	//Life -------------
	uint			max_life = 0;
	int				life = 0;
	iPoint			life_position = { 0,0 };
	//Visual -----------
	Animation*		current_animation = nullptr;
	SDL_Color		blit_color = { 255,255,255,255 };
	bool			flip_sprite = false;
	SDL_Rect		selection_rect = { 0,0,0,0 };
	SDL_Rect		icon_rect = { 0,0,0,0 };
	//Worker -----------
	ActionWorker	action_worker;

	//Fog Tiles --------
	std::vector<AlphaCell*> in_vision_cells;
	std::vector<FogTile*>	in_vision_tiles;

public:

	//Functionality -------------------
	//Save/Load methods ----
	virtual void	SaveAsDef(pugi::xml_node& node) = 0;

	//Select/Deselect ------
	void			Select();
	void			Deselect();

	//Operators -------------
	bool operator == (const Entity& tar);

	//Update ----------------
	virtual bool	Update();
	virtual bool	Die();

	//Draw ------------------
	virtual bool	Draw(bool debug);
	void			DrawLife();
	void			CleanAnimation();
	virtual void	CleanMapLogic();
	void			SetBlitColor(const SDL_Color new_color);

	virtual void	ResetFogAround();
	void			CleanFogAround();
	void			CheckFogAround();

	//Add Action ------------
	void			AddAction(Action* action, TASK_CHANNELS channel = TASK_CHANNELS::PRIMARY);
	void			AddPriorizedAction(Action* action, TASK_CHANNELS channel = TASK_CHANNELS::PRIMARY);
	void			PopAction(Action* action);

	//Set Methods -----------
	void			SetName(const char* name_str);
	virtual void	SetPosition(float x, float y, bool insert = true);
	virtual void	OnlySetPosition(float x, float y);
	void			SetEntityType(ENTITY_TYPE type);
	void			SetDiplomacy(DIPLOMACY new_diplomacy);
	void			SetVision(const Circle& new_mark);
	void			SetRenderArea(const Circle& new_render_area);
	void			SetMaxLife(uint full_life_val);
	void			SetLife(uint life_val);
	void			SetLifeBarPosition(int x, int y);
	void			SetAnimation(Animation* anim);
	void			SetFlipSprite(bool flip);
	void			SetSelectionRect(const SDL_Rect& rect);
	void			SetIcon(const SDL_Rect& icon);

	//Get Methods -----------
	const char*		GetName()const;
	const fPoint&	GetPosition()const;
	iPoint			GetPositionRounded()const;
	ENTITY_TYPE		GetEntityType()const;
	DIPLOMACY		GetDiplomacy()const;
	Circle			GetVision()const;
	Circle			GetRenderArea()const;
	virtual uint	GetMaxLife()const;
	int				GetLife()const;
	Animation*		GetAnimation();
	bool			GetFlipSprite()const;
	bool			GetIfSelected()const;
	const SDL_Rect*	GetSelectionRect()const;
	const SDL_Rect&	GetIcon()const;
	ActionWorker*	GetWorker();
};
/// ---------------------------------------------

///Class Unit -----------------------------------
//Base class that define the general attributes for all units
class Unit : public Entity
{
public:

	Unit();
	Unit(const Unit& copy);

	~Unit();

protected:

	//Stats ----------------------
	UNIT_TYPE		unit_type = NO_UNIT;
	UNIT_CLASS		unit_class = NO_CLASS;
	Circle			mark;
	Circle			soft_collider;
	Circle			hard_collider;

	//Movement ---------
	float			speed = 0.0f;
	float			mutable_speed = 0.0f;
	float			speed_buff = 0.0f;
	int				current_distance = 0;
	ACTION_TYPE		action_type = IDLE;
	DIRECTION_TYPE	direction_type = SOUTH;
	iPoint			future_position = { 0,0 };
	iPoint			last_position = { 0,0 };
	fPoint			distance_walked = { 0.0f , 0.0f };
	//Attack -----------
	j1Timer			action_timer;
	uint			attack_hitpoints = 0;
	float			attack_buff = 0;
	uint			attack_area_base = 0;
	uint			attack_rate = 0;
	uint			attack_rate_buff = 0;

	ATTACK_TYPE		attack_type = NO_ATTACK;
	//Defense  ---------
	uint			defense = 0; /*Used in melee damage*/
	float			defense_buff = 0;
	//Resources --------
	uint			food_cost = 0;
	uint			wood_cost = 0;
	uint			gold_cost = 0;
	uint			population_cost = 0;
	uint			train_time = 0;
	uint			exp = 0;
	//Vision
	float				vision_base = 0;
	//Life
	float				life_buff = 0;
	//Others
	uint			unit_experience = 0;
	bool			unit_protected = false;
	Warrior*		tank = nullptr;

	// Attack area
	Circle	attack_area;

public:

	float distance_to_target = 0;

public:

	//Functionality -------------------
	//Save/Load methods ----
	void	SaveAsDef(pugi::xml_node& node);

	//Operator --------------
	bool operator<(const Unit& target) const;

	//Draw ------------------
	bool	Draw(bool debug);

	//Actions ---------------
	bool					Move(std::vector<iPoint>*& path, const iPoint& target = { -1,-1 },Unit* unit_target=nullptr);
	void					ResetDistance();
	void					ToIdle();
	iPoint					Spawn( const iPoint& target = { -1,-1 });

	bool					Repath(std::vector<iPoint>*& path, const iPoint & destination);

	void					CorrectPath(std::vector<iPoint>*& path);
	iPoint					NextGoal(std::vector<iPoint>* path);
	void					NewPosition(const iPoint& goal, float& position_x, float& position_y);
	iPoint					FindWalkableCell(const iPoint& center);
	iPoint					FindSpawnCell(const iPoint& center);

	bool					UnitHere(const iPoint& destination, int radius);
	bool					SurroundedUnit();
	iPoint					FindWalkableAdjacent(const iPoint& center);

	Unit*					FindNewTarget();
	void					Focus(const iPoint& target, bool play = true);
	DIRECTION_TYPE			LookDirection(const iPoint& from, const iPoint& to);
	bool					AttackUnit(Unit* target);
	bool					HealUnit(Unit* target);
	bool					AttackBuilding(Building* target);
	bool					Cover();
	bool					DirectDamage(uint damage);
	void					Heal(uint heal);

	bool					Die();
	bool					Resurrect();
	void					Stun(uint time);
	COLLISION_TYPE			CheckColision(const Unit* current, const Unit* other);
	void					ResetFogAround();

	//Set Quit Protection
	void	SetProtection(Warrior* warrior);
	void	QuitProtection();

	//Set Methods -----------
	void	SetPosition(float x, float y, bool insert = true);
	void	SetFutureAction(const iPoint& future_position);
	void	SetUnitType(UNIT_TYPE type);
	void	SetUnitClass(UNIT_CLASS type);

	void	SetMark(const Circle& new_mark);
	void	SetSoftCollider(const Circle& new_soft_collider);
	void	SetHardCollider(const Circle& new_hard_collider);
	void	SetSpeed(float speed_val);
	void	SetSpeedBuff(float speed_val);

	void	SetAction(ACTION_TYPE action_val);
	void	SetDirection(DIRECTION_TYPE direction_val);
	void	SetAttackHitPoints(uint atk_val);
	void	SetAttackBuff(float atk_buff);
	void	SetAttackRate(uint atk_rate);
	void	SetAttackRateBuff(uint atk_rate);

	void	SetAttackType(ATTACK_TYPE atk_type);
	void	SetAttackArea(const Circle& atk_area);

	void	SetRangeBase(uint atk_range);

	void	SetDefense(uint def);
	void	SetDefenseBuff(float def_buff);
	void	SetVisionBase(float vis_buff);
	void	SetLifeBuff(float hp_buff);
	void	SetFoodCost(uint food_cst);
	void	SetWoodCost(uint wood_cst);
	void	SetGoldCost(uint coin_cst);
	void	SetPopulationCost(uint poblation_cst);
	void	SetTrainTime(uint train_time_val);
	void	SetExp(uint experience);
	void	SetUpgrade(Unit* previous_unit);
	void	SetUnitExperience(uint value);
	//Get Methods -----------
	UNIT_TYPE		GetUnitType()const;
	UNIT_CLASS		GetUnitClass()const;
	const Circle&	GetMark()const;
	const Circle&	GetSoftCollider()const;
	const Circle&	GetHardCollider()const;

	float			GetSpeed()const;
	float			GetSpeedBuff()const;
	float			GetTotalSpeed()const;
	ACTION_TYPE		GetAction()const;
	DIRECTION_TYPE	GetDirection()const;
	uint			GetAttackHitPoints()const;
	float			GetAttackBuff()const;
	uint			GetAttackRate()const;
	uint			GetAttackRateBuff()const;

	ATTACK_TYPE		GetAttackType()const;
	uint			GetAttackRange()const;

	const Circle*	GetAttackArea()const;
	uint			GetAttackAreaBase()const;

	uint			GetDefense()const;
	float			GetDefenseBuff()const;
	uint			GetFoodCost()const;
	uint			GetWoodCost()const;
	uint			GetGoldCost()const;
	uint			GetPopulationCost()const;
	uint			GetTrainTime()const;
	uint			GetExp()const;
	std::vector<iPoint>* GetPath() const;
	iPoint			GetFuturePosition() const;
	uint			GetMaxLife() const;
	uint			GetTotalMaxLife() const;
	uint			GetBaseMaxLife() const;

	float			GetVisionBase()const;
	float			GetLifeBuff()const;


	void ShotArrow(fPoint start, fPoint goal);

};
/// ---------------------------------------------

///Class Resource -------------------------------
//Base class that defines standard attributes of resources
class Resource : public Entity
{
public:

	Resource();
	Resource(const Resource& copy);
	~Resource();

protected:

	Rectng			mark;
	Circle			interact_area;
	RESOURCE_TYPE	resource_type = NO_RESOURCE;

	void CleanMapLogic();

public:

	//Functionality -------------------
	//Save/Load methods ----
	void	SaveAsDef(pugi::xml_node& node);

	//Draw ------------------
	bool	Draw(bool debug);

	//State -----------------
	virtual bool ExtractResources(uint* value);

	//Set Methods -----------
	void	SetPosition(float x, float y, bool insert = true);
	void	SetMark(const Rectng& rectangle);
	void	SetInteractArea(const Circle& area);
	void	SetResourceType(RESOURCE_TYPE type);

	//Get Methods -----------
	const Rectng&	GetMark()const;
	const Circle*	GetInteractArea()const;
	RESOURCE_TYPE	GetResourceType()const;

};
/// ---------------------------------------------

///Class Building -------------------------------
//Base class that defines standard attributes of buildings
class Building : public Entity
{
public:

	Building();
	Building(const Building& copy);
	~Building();

protected:

	Rectng			mark;
	Circle			interact_area;
	uint			width_in_tiles = 0;
	uint			height_in_tiles = 0;
	BUILDING_TYPE	building_type = NO_BUILDING;
	ACTION_TYPE		action_type = IDLE;
	DIRECTION_TYPE	direction_type = NO_DIRECTION;
	bool			fog_discovered = false;

public:

	void CleanMapLogic();
	void ResetFogAround();
	void DiscoverFogAround();

	//Functionality -------------------
	//Save/Load methods ----
	void	SaveAsDef(pugi::xml_node& node);

	//Check is can be built in the position
	bool	CheckZone(int x, int y);

	//Actions ---------------
	bool	Die();

	//Game Loop -------------
	bool	Draw(bool debug);
	bool	Update();

	//Set Methods -----------
	void	SetPosition(float x, float y, bool insert = true);
	void	OnlySetPosition(float x, float y);
	void	SetMark(const Rectng& rectangle);
	void	SetInteractArea(const Circle& rectangle);
	void	SetWidthInTiles(uint width);
	void	SetHeightInTiles(uint height);
	void	SetBuildingType(BUILDING_TYPE type);
	void	SetActionType(ACTION_TYPE type);
	void	SetDirectionType(DIRECTION_TYPE type);
	void	SetFogDiscovered(bool val);
	void	SetUpgrade(Building* new_building);

	//Get Methods -----------
	const Rectng&	GetMark()const;
	const Circle*	GetInteractArea()const;
	BUILDING_TYPE	GetBuildingType()const;
	ACTION_TYPE		GetActionType()const;
	DIRECTION_TYPE	GetDirectionType()const;
	bool			GetFogDiscovered()const;
};
/// ---------------------------------------------
#endif // _ENTITY_

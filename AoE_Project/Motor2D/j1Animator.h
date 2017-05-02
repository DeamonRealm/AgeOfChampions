#ifndef _J1ANIMATOR_H_
#define _J1ANIMATOR_H_

#include "j1Module.h"
#include "j1Timer.h"
#include "p2Point.h"
#include "SDL/include/SDL_rect.h"
struct SDL_Texture;
class Unit;
class Building;
class Resource;
enum UNIT_TYPE;
enum ACTION_TYPE;
enum DIRECTION_TYPE;
enum BUILDING_TYPE;
enum RESOURCE_TYPE;
enum ATTACK_TYPE;
enum ENTITY_TYPE;
enum ITEM_TYPE;
enum DIPLOMACY;

///Animation Sprite Class -----------------------
class Sprite
{
public:

	Sprite(const SDL_Rect& frame, const iPoint& pivot, int z_cord = 0, uint opacity = 255);
	~Sprite();

private:

	SDL_Rect		frame = { 0,0,0,0 };
	iPoint			pivot = { 0,0 };
	int				z_cord = 0;
	uint			opacity = 255;

public:

	//Functionality
	//Get Methods
	const SDL_Rect*		GetFrame()const;
	int					GetXpivot()const;
	int					GetYpivot()const;
	int					GetZ_cord()const;
	uint				GetOpacity()const;

};
/// ---------------------------------------------

///Animation Class ------------------------------
//Class that contains the animation basic necessary data
class Animation
{
public:

	Animation();
	virtual ~Animation();

protected:

	//Vector that storage the frames rect & pivot & z dimension
	std::vector<Sprite>		sprites;
	//Pointer to the animation texture
	SDL_Texture*			texture = nullptr;
	//Id of the animation enum type
	uint					enum_id = 0;
	//Current frame calculated by the timer
	float					current_frame = 0;
	bool					loop = true;
	int						loops = 0;
	//Animation update rate
	uint					speed = 300;
	j1Timer					timer;

public:

	//Functionality -------------------
	//Called before clean ---
	virtual void	ReleaseTexture();

	//Set Methods -----------
	void	SetTexture(const SDL_Texture* tex);
	void	SetLoop(bool loop_state);
	void	SetCurrentFrame(uint curr);
	void	SetSpeed(uint new_speed);
	void	SetId(uint id);

	//Get Methods -----------
	SDL_Texture*					GetTexture()const;
	bool							GetLoop()const;
	uint							GetCurrentFrame()const;
	uint							GetSpeed()const;
	const Sprite*					GetCurrentSprite();
	const std::vector<Sprite>*		GetAllSprites()const;
	uint							GetId()const;

	//Check if the animation is end
	bool IsEnd();
	//Reset the animation frame
	void Reset();

	//Add New sprite
	void AddSprite(const SDL_Rect& rect, const iPoint& point, const int z = 0, const uint opacity = 255);

};
/// ---------------------------------------------

/// Animation Block Class -----------------------
//Block that contains a vector of animations 
class Animation_Block
{
public:

	Animation_Block(uint enum_id = 0);

	~Animation_Block();

protected:

	//Vector of other animation blocks
	std::vector<Animation_Block*>	childs;
	//Enum id of this block
	uint							enum_id = 0;
	//Pointer to a vector of animations 
	Animation*						animation = nullptr;

public:

	//Functionality -------------------
	//Delete all contained blocks data
	void	ClearAnimationBlocks();

	//Set Methods -----------
	void	SetId(uint id);

	//Get Methods -----------
	uint				GetId()const;
	Animation*			GetAnimation()const;
	Animation_Block*	GetBlock(int index)const;
	uint				GetChildsNum()const;
	Animation_Block*	SearchId(uint id)const;

	//Add Methods -----------
	void	SetAnimation(const Animation* new_animation);
	void	AddAnimationBlock(Animation_Block* new_animation_block);

};
/// ---------------------------------------------

///Diplomatic Animation Class ------------------------------
//Class that contains the animation basic necessary data
class DiplomaticAnimation : public Animation
{
public:

	DiplomaticAnimation();
	~DiplomaticAnimation();

private:

	//Pointer to the animation different diplomacy texture
	SDL_Texture*			red_texture = nullptr;

public:

	//Functionality -------------------
	//Called before clean ---
	void	ReleaseTexture();

	//Set Methods -----------
	void	SetRedTexture(const SDL_Texture* tex);

	//Get Methods -----------
	SDL_Texture*					GetTexture(DIPLOMACY target_diplomacy)const;
	SDL_Texture*					GetRedTexture()const;

};
/// ---------------------------------------------

/// Animation Block Class -----------------------
//Block that contains a vector of animations 
class DiplomaticAnimation_Block
{
public:

	DiplomaticAnimation_Block(uint enum_id = 0);

	~DiplomaticAnimation_Block();

private:

	//Vector of other animation blocks
	std::vector<DiplomaticAnimation_Block*>	childs;
	//Enum id of this block
	uint						enum_id = 0;
	//Pointer to a vector of animations 
	DiplomaticAnimation*		animation = nullptr;

public:

	//Functionality -------------------
	//Delete all contained blocks data
	void	ClearAnimationBlocks();

	//Set Methods -----------
	void	SetId(uint id);

	//Get Methods -----------
	uint						GetId()const;
	DiplomaticAnimation*		GetAnimation()const;
	DiplomaticAnimation_Block*	GetBlock(int index)const;
	uint						GetChildsNum()const;
	DiplomaticAnimation_Block*	SearchId(uint id)const;

	//Add Methods -----------
	void	SetAnimation(const DiplomaticAnimation* new_animation);
	void	AddAnimationBlock(DiplomaticAnimation_Block* new_animation_block);

};
/// ---------------------------------------------

//Animator Module -------------------------------
class j1Animator : public j1Module
{
public:

	j1Animator();
	~j1Animator();

public:

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

private:

	//Vectors that define all the entities animations
	std::vector<DiplomaticAnimation_Block*> unit_blocks;
	std::vector<DiplomaticAnimation_Block*>	building_blocks;
	std::vector<Animation_Block*>			resource_blocks;

public:

	//Functionality -------------------
	//Methods that transform strings to enums (used when loading data from xml)
	ENTITY_TYPE		StrToEntityEnum(const char* str)const;
	DIPLOMACY		StrToDiplomacyEnum(const char* str)const;
	UNIT_TYPE		StrToUnitEnum(const char* str)const;
	ACTION_TYPE		StrToActionEnum(const char* str)const;
	ITEM_TYPE		StrToItemEnum(const char* str)const;
	DIRECTION_TYPE	StrToDirectionEnum(const char* str)const;
	BUILDING_TYPE	StrToBuildingEnum(const char* str)const;
	ATTACK_TYPE		StrToAttackEnum(const char* str)const;
	RESOURCE_TYPE	StrToResourceEnum(const char* str)const;

	//Get the loop boolean from the action animation type
	bool			AnimationLoopFromActionType(ACTION_TYPE type);

	//Blocks Load Methods ---
	bool		LoadUnitBlock(const char* xml_folder);
	bool		LoadVillagerBlock(pugi::xml_document* doc);
	bool		LoadBuildingBlock(const char* xml_folder);
	bool		LoadResourceBlock(const char* xml_folder);

	//Animations Methods ----
	bool UnitPlay(Unit* target);
	bool BuildingPlay(Building* target);
	bool ResourcePlay(Resource* target);

};
#endif // !_J1ANIMATOR_H_

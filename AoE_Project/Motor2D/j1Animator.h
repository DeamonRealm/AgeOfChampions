#ifndef _J1ANIMATOR_H_
#define _J1ANIMATOR_H_

#include "j1Module.h"
#include "j1Timer.h"
#include "p2Point.h"

struct SDL_Rect;
struct SDL_Texture;
struct Unit;
enum UNIT_TYPE;
enum ACTION_TYPE;
enum DIRECTION_TYPE;
enum BUILDING_TYPE;

///Animation Class ------------------------------
//Class that contains the animation basic necessary data
class Animation
{
public:

	Animation();

	~Animation();

private:

	//Vectors that storage the frames rect & pivot
	std::vector<SDL_Rect>	frames;
	std::vector<iPoint>		pivots;
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
	//Set Methods -----------
	void	SetTexture(const SDL_Texture* tex);
	void	SetLoop(bool loop_state);
	void	SetSpeed(uint new_speed);
	void	SetId(uint id);

	//Get Methods -----------
	SDL_Texture*					GetTexture()const;
	bool							GetLoop()const;
	uint							GetSpeed()const;
	const SDL_Rect&					GetCurrentFrame();
	const std::vector<SDL_Rect>*	GetAllFrames()const;
	const iPoint&					GetCurrentPivot()const;
	const std::vector<iPoint>*		GetAllPivots()const;
	uint							GetId()const;

	//Add New frame with pivot defined
	void AddFrame(const SDL_Rect& rect, const iPoint& point);

};
/// ---------------------------------------------

/// Animation Block Class -----------------------
//Block that contains a vector of animations 
class Animation_Block
{
public:

	Animation_Block(uint enum_id = 0);

	~Animation_Block();

private:

	//Vector of other animation blocks
	std::vector<Animation_Block*>	childs;
	//Enum id of this block
	uint							enum_id = 0;
	//Pointer to a vector of animations 
	Animation*		animation;

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
	std::vector<Animation_Block*> unit_blocks;
	std::vector<Animation_Block*> building_blocks;
	std::vector<Animation_Block*> resource_blocks;

	//Methods that transform strings to enums (used when loading data from xml)
	UNIT_TYPE		Str_to_UnitEnum(const char* str)const;
	ACTION_TYPE		Str_to_ActionEnum(const char* str)const;
	DIRECTION_TYPE	Str_to_DirectionEnum(const char* str)const;
	BUILDING_TYPE	Str_to_BuildingEnum(const char* str)const;

public:

	//Functionality -------------------
	//Load Civilization -----
	bool		LoadCivilization(const char* folder);
	//Blocks Load Methods ---
	bool		LoadUnitBlock(const char* xml_folder, const char* tex_folder);
	bool		LoadBuildingBlock(const char* xml_folder, const char* tex_folder);
	//bool		LoadResourceBlock(const char* folder);

	//Animations Methods ----
	bool UnitPlay(Unit* target);
	Animation* BuildingPlay(const BUILDING_TYPE unit, const ACTION_TYPE action, const DIRECTION_TYPE direction)const;
};
#endif // !_J1ANIMATOR_H_

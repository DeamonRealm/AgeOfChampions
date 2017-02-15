#ifndef _J1ANIMATOR_H_
#define _J1ANIMATOR_H_

#include "j1Module.h"
#include "j1Timer.h"
#include "p2Point.h"

struct SDL_Rect;

enum UNIT_TYPE
{
	MILITIA
};
enum ACTION_TYPE
{
	ATTATCK,
	DIE,
	DISAPEAR,
	IDLE,
	WALK
};
enum DIRECTION_TYPE
{
	NORTH
};

///Animation Class ------------------------------
//Class that contains the animation basic necessary data
class Animation
{
public:

	Animation();

	~Animation();

private:

	std::vector<SDL_Rect>	frames;
	std::vector<iPoint>		pivots;
	float					current_frame = 0;
	int						loops = 0;
	bool					loop = true;
	uint					speed = 300;
	j1Timer					timer;

public:

	//Set Methods -----------
	void	SetLoop(bool loop_state);
	void	SetSpeed(uint new_speed);

	//Get Methods -----------
	bool			GetLoop()const;
	uint			GetSpeed()const;
	const SDL_Rect&	GetCurrentFrame();

	//Add New frame with pivot defined
	void AddFrame(const SDL_Rect& rect, const iPoint& point);

};
/// ---------------------------------------------

/// Animation Block Class -----------------------
//Block that contains a vector of animations 
template<class ENUM>
class Animation_Block
{
public:

	Animation_Block();

	Animation_Block(const ENUM& type);

	~Animation_Block();

private:

	//Vector of other animation blocks
	std::vector<Animation_Block*> childs;

	//Enum of this block
	ENUM				type;

	//Vector of animations 
	std::vector<Animation*>	animations;

public:

	//Delete all contained blocks data
	void	ClearAnimationBlocks();

	//Set Methods -----------
	bool	SetEnum(const ENUM& new_enum);

	//Get Methods -----------
	ENUM    GetEnum()const;
};
typedef Animation_Block<UNIT_TYPE> Unit_Animation_Block;
typedef Animation_Block<ACTION_TYPE> Action_Animation_Block;
typedef Animation_Block<DIRECTION_TYPE> Direction_Animation_Block;
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

	// Called before quitting
	bool CleanUp();

private:

	std::vector<Unit_Animation_Block*> animation_blocks;

	//Methods that transform strings to enums (used when loading data from xml)
	UNIT_TYPE		Str_to_UnitEnum(const char* str)const;
	ACTION_TYPE		Str_to_ActionEnum(const char* str)const;
	DIRECTION_TYPE	Str_to_DirectionEnum(const char* str)const;

public:

	//bool Play(/* Unit* */)const;
};
#endif // !_J1ANIMATOR_H_

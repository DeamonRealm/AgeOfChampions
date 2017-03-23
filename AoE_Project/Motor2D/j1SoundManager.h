#ifndef _J1SOUND_MANAGER_H_
#define  _J1SOUND_MANAGER_H_

#include "j1Module.h"
#include "j1Timer.h"
#include "p2Point.h"
#include "SDL/include/SDL_rect.h"
struct SDL_Texture;
class Unit;
class Building;
class Resource;
enum SOUND_ATTACK_TYPE;
enum SOUND_TYPE;
///Animation Sprite Class -----------------------
class Fx
{
public:

	Fx(const int sound_id, const int loops);
	~Fx();

private:

	
	int						id = 255;
	int						loops = 0;


public:

	//Functionality
	//Get Methods
	

};
/// ---------------------------------------------

///Animation Class ------------------------------
//Class that contains the animation basic necessary data
class Sound
{
public:

	Sound();

	~Sound();

private:

	//Vector that storage the frames rect & pivot & z dimension
	std::vector<int>		sound;
	//Id of the animation enum type
	uint					enum_id = 0;
	//Current frame calculated by the timer
	int						loops = 0;
public:

	//Functionality -------------------
	//Set Methods -----------
	void	SetLoop(int loops);
	void	SetId(uint id);

	//Get Methods -----------
	int								GetLoop()const;
	const std::vector<int>*			GetAllSouds()const;
	uint							GetId()const;

	//Add New sprite
	void AddSprite(const int sound_id, const int loops);

};
/// ---------------------------------------------

/// Animation Block Class -----------------------
//Block that contains a vector of animations 
class Sound_Block
{
public:

	Sound_Block(uint enum_id = 0);

	~Sound_Block();

private:

	//Vector of other animation blocks
	std::vector<Sound_Block*>	childs;
	//Enum id of this block
	uint							enum_id = 0;
	//Pointer to a vector of animations 
	Sound*		sound = nullptr;

public:

	//Functionality -------------------
	//Delete all contained blocks data
	void	ClearAnimationBlocks();

	//Set Methods -----------
	void	SetId(uint id);

	//Get Methods -----------
	uint				GetId()const;
	Sound*			GetAnimation()const;
	Sound_Block*	GetBlock(int index)const;
	uint				GetChildsNum()const;
	Sound_Block*	SearchId(uint id)const;

	//Add Methods -----------
	void	SetAnimation(const Sound* new_animation);
	void	AddAnimationBlock(Sound_Block* new_animation_block);

};
/// ---------------------------------------------


//Animator Module -------------------------------
class j1SoundManager : public j1Module
{
public:

	j1SoundManager();
	~j1SoundManager();

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
	std::vector<Sound_Block*> sound_block;

public:

	//Functionality -------------------
	//Methods that transform strings to enums (used when loading data from xml)
	SOUND_ATTACK_TYPE		StrToSoundEnum(const char* str)const;
	SOUND_TYPE				StrToAttackEnum(const char* str)const;


	//Blocks Load Methods ---
	bool		LoadSoundBlock(const char* xml_folder);


	//Animations Methods ----
	bool UnitPlay(Unit* target);
	bool BuildingPlay(Building* target);
	bool ResourcePlay(Resource* target);

};
#endif // ! _J1SOUND_MANAGER_H_

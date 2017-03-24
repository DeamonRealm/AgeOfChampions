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


/// Animation Block Class -----------------------
//Block that contains a vector of animations 
class Sound_Block
{
public:

	Sound_Block(uint enum_id = 0);

	~Sound_Block();

private:

	//Enum id of this block
	uint						enum_id = 0;
	//Pointer to a vector of animations 
	std::vector<int>		sound;

public:

	//Functionality -------------------
	void	ClearSound();
	//Set Methods -----------
	void	SetId(uint id);
	void	SetSound(const int value);
	//Get Methods -----------
	int					GetAudio(int index);

	int					GetSoundSize();
	uint				GetId()const;
	
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
	std::vector<Sound_Block*> sound_blocks;

public:

	//Functionality -------------------
	//Methods that transform strings to enums (used when loading data from xml)
	SOUND_ATTACK_TYPE		StrToSoundEnum(const char* str)const;


	//Blocks Load Methods ---
	bool		LoadSoundBlock(const char* xml_folder);


	//Animations Methods ----
	bool PlayAudio(SOUND_TYPE target);
	

};
#endif // ! _J1SOUND_MANAGER_H_

#ifndef _J1SOUND_MANAGER_H_
#define  _J1SOUND_MANAGER_H_

#include "j1Module.h"
#include "j1Timer.h"
#include "BaseEntities.h"
enum SOUND_TYPE;


/// Animation Block Class -----------------------
//Block that contains a vector of animations 
class Sound_Block
{
public:

	Sound_Block(SOUND_TYPE type = NO_SOUND);

	~Sound_Block();
	std::string				path;

private:

	//Enum id of this block
	SOUND_TYPE				type = NO_SOUND;
	//Pointer to a vector of animations 
	std::vector<int>		sound;
public:

	//Functionality -------------------
	void				ClearSound();
	//Set Methods -----------
	void				SetType(SOUND_TYPE type);
	void				SetSound(const int value);
	void				SetString(const char* path);
	//Get Methods -----------
	int					GetAudio(int index);
	int					GetSoundSize();
	SOUND_TYPE			GetType()const;

	const char*			GetPath();
	
	
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
	std::vector<Sound_Block*> fx_blocks;
	std::vector<Sound_Block*> music_blocks;

public:

	//Functionality -------------------
	//Methods that transform strings to enums (used when loading data from xml)
	SOUND_TYPE		StrToSoundEnum(const char* str)const;


	//Blocks Load Methods ---
	bool		LoadSoundBlock(const char* xml_folder);


	//Animations Methods ----
	bool PlayFXAudio(SOUND_TYPE target);
	bool PlayMusicAudio(SOUND_TYPE target);


};
#endif // ! _J1SOUND_MANAGER_H_

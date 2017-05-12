#ifndef _J1SOUND_MANAGER_H_
#define  _J1SOUND_MANAGER_H_

#include "j1Module.h"
#include "j1Timer.h"
#include "BaseEntities.h"
#define ARROW_SOUND_TIME 500
#define SWORD_SOUND_TIME 500
#define HEAL_SOUND_TIME 500
#define ALERT_SOUND_TIME 5000

enum SOUND_TYPE;

struct Sound
{
	Sound(int id, std::string path);
	~Sound();

	int id = 0;
	std::string	path;
};
/// Animation Block Class -----------------------
//Block that contains a vector of animations 
class Sound_Block
{
public:

	Sound_Block(SOUND_TYPE type = NO_SOUND);

	~Sound_Block();

private:

	//Enum id of this block
	SOUND_TYPE				type = NO_SOUND;
	//Pointer to a vector of animations 
	std::vector<Sound*>		sound;
public:

	//Functionality -------------------
	void				ClearSound();
	//Set Methods -----------
	void				SetType(SOUND_TYPE type);
	void				SetSound(const int value,const char* string);
	//Get Methods -----------
	Sound*				GetAudio(int index);
	int					GetSoundSize();
	SOUND_TYPE			GetType()const;

	
	
};
/// ---------------------------------------------


//Animator Module -------------------------------
class j1SoundManager : public j1Module
{
public:

	j1SoundManager();
	~j1SoundManager();

public:

	
	bool Enable();
	void Disable();

	bool Start();

	// Called before quitting
	bool CleanUp();

private:

	//Vectors that define all the entities animations
	std::vector<Sound_Block*> fx_blocks;
	std::vector<Sound_Block*> music_blocks;
	std::vector<Sound_Block*> gui_blocks;

	j1Timer arrow_sound;
	j1Timer sword_sound;
	j1Timer heal_sound;
	j1Timer alert_sound;

public:

	//Functionality -------------------
	//Methods that transform strings to enums (used when loading data from xml)
	SOUND_TYPE		StrToSoundEnum(const char* str)const;

	void			CheckAttackSound(iPoint location,ATTACK_TYPE attack_type,bool heal = false);
	void			CheckAlertSound(iPoint location, bool building = false);

	//Blocks Load Methods ---
	bool			LoadSoundBlock(const char* xml_folder);


	//Animations Methods ----
	bool			PlayFXAudio(SOUND_TYPE target);
	bool			PlayGUIAudio(SOUND_TYPE target);
	bool			PlayMusicAudio(SOUND_TYPE target);


};
#endif // ! _J1SOUND_MANAGER_H_

#include "j1SoundManager.h"

#include <time.h>
#include <stdlib.h>

#include "j1App.h"
#include "j1FileSystem.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "SDL/include/SDL_rect.h"
#include "p2Log.h"
#include "j1EntitiesManager.h"
#include "j1Audio.h"
///Animation Sprite Class -----------------------
//Constructor ===============
Fx::Fx(const int sound_id, const int loops) : id(sound_id), loops(loops)
{

}

//Destructor ================
Fx::~Fx()
{
}

//Functionality =============

/// ---------------------------------------------



///Animation Class ------------------------------
//Constructor =========================
Sound::Sound()
{

}

//Destructor ==========================
Sound::~Sound()
{
	sound.clear();
}



//Functionality =======================
void Sound::SetLoop(int loop_state)
{
	loops = loop_state;
}

void Sound::SetId(uint id)
{
	enum_id = id;
}


int Sound::GetLoop() const
{
	return loops;
}


const std::vector<int>* Sound::GetAllSouds() const
{
	return &sound;
}

uint Sound::GetId() const
{
	return enum_id;
}

void Sound::AddSprite(const int sound_id, const int loops)
{
	sound.push_back(sound_id);
}
/// ---------------------------------------------


/// Animation Block Class -----------------------
//Constructor =========================
Sound_Block::Sound_Block(uint enum_id) :enum_id(enum_id)
{

}

//Destructor ==========================
Sound_Block::~Sound_Block()
{
	ClearAnimationBlocks();
}

//Functionality =======================
void Sound_Block::ClearAnimationBlocks()
{
	while (childs.size() > 0)
	{
		childs.back()->ClearAnimationBlocks();
		childs.pop_back();

	}

	if (sound != nullptr)delete sound;
}

void Sound_Block::SetId(uint id)
{
	enum_id = id;
}

uint Sound_Block::GetId() const
{
	return enum_id;
}

Sound* Sound_Block::GetAnimation() const
{
	return sound;
}

Sound_Block * Sound_Block::GetBlock(int index) const
{
	return childs.at(index);
}

uint Sound_Block::GetChildsNum() const
{
	return childs.size();
}

Sound_Block * Sound_Block::SearchId(uint id) const
{
	uint size = childs.size();
	for (uint k = 0; k < size; k++)
	{
		if (childs[k]->GetId() == id)return childs[k];
	}

	return nullptr;
}

void Sound_Block::SetAnimation(const Sound * new_animation)
{
	if (new_animation != nullptr)sound = (Sound*)new_animation;
}

void Sound_Block::AddAnimationBlock(Sound_Block* new_animation_block)
{
	if (new_animation_block != nullptr)childs.push_back(new_animation_block);
}
/// ---------------------------------------------


//Animator Module -------------------------------
//Constructor =========================
j1SoundManager::j1SoundManager()
{
	name = "sounds";
}

//Destructor ==========================
j1SoundManager::~j1SoundManager()
{
}

//Game Loop ===========================
bool j1SoundManager::Awake(pugi::xml_node& config)
{
	return true;
}

bool j1SoundManager::Start()
{
	return true;
}

bool j1SoundManager::PostUpdate()
{
	return true;
}

bool j1SoundManager::CleanUp()
{
	//Clean the unit blocks
	uint size = sound_block.size();

	for (uint k = 0; k < size; k++)
	{
		sound_block[k]->ClearAnimationBlocks();
	}
	sound_block.clear();
	return true;
}


//Methods that transform strings to enums (used when loading data from xml)
SOUND_ATTACK_TYPE j1SoundManager::StrToSoundEnum(const char * str) const
{
	if (strcmp(str, "sword") == 0)		return SWORD_ATTACK;

	return NO_ATTACK_SOUND;
}

SOUND_TYPE j1SoundManager::StrToAttackEnum(const char * str) const
{
	if (strcmp(str, "attack") == 0)		return ATTACK_SOUND;

	return NO_SOUND;
}


//Functionality =======================
bool j1SoundManager::LoadSoundBlock(const char* xml_folder)
{
	

	return true;
}

bool j1SoundManager::UnitPlay(Unit* target)
{

	
	return false;
}

bool j1SoundManager::BuildingPlay(Building* target)
{


	return false;
}

bool j1SoundManager::ResourcePlay(Resource * target)
{
	
	return false;
}

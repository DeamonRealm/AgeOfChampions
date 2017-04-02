#include "j1SoundManager.h"

#include <time.h>
#include <stdlib.h>

#include "j1FileSystem.h"
#include "p2Log.h"
#include "j1Audio.h"
#include "j1EntitiesManager.h"
#include "j1App.h"


/// Sound Block Class -----------------------
//Constructor =========================
Sound_Block::Sound_Block(SOUND_TYPE type) :type(type)
{

}

//Destructor ==========================
Sound_Block::~Sound_Block()
{
	
}

//Functionality =======================

void Sound_Block::ClearSound()
{
	sound.clear();
}

void Sound_Block::SetType(SOUND_TYPE set)
{
	type = set;
}

void Sound_Block::SetSound(const int value)
{
	sound.push_back(value);
}

void Sound_Block::SetString(const char * get)
{
	path = get;
}

int Sound_Block::GetAudio(int index)
{
	return sound[index];
}



int Sound_Block::GetSoundSize()
{
	return sound.size();
}

SOUND_TYPE Sound_Block::GetType() const
{
	return type;
}

const char * Sound_Block::GetPath()
{
	return path.c_str();
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
	uint music_size = music_blocks.size();

	for (uint k = 0; k < music_size; k++)
	{
		music_blocks[k]->ClearSound();
	}
	music_blocks.clear();

	uint fx_size = fx_blocks.size();

	for (uint k = 0; k < fx_size; k++)
	{
		fx_blocks[k]->ClearSound();
	}
	fx_blocks.clear();
	return true;
}


//Methods that transform strings to enums (used when loading data from xml)
SOUND_TYPE j1SoundManager::StrToSoundEnum(const char * str) const
{
	if		(strcmp(str, "sword_solo") == 0)					return SWORD_ATTACK_SOUND;
	else if (strcmp(str, "barracks_create_select") == 0)		return BARRACK_SOUND;
	else if (strcmp(str, "town_center_select") == 0)			return TOWN_CENTER_SELECTED_SOUND;
	else if (strcmp(str, "villager_create") == 0)				return VILLAGER_CREATED_SOUND;
	else if (strcmp(str, "unit_death") == 0)					return DEATH_SOUND;
	else if (strcmp(str, "unit_attack") == 0)					return ATTACK_SOUND;
	else if (strcmp(str, "unit_click") == 0)					return CLICK_SOUND;
	else if (strcmp(str, "unit_move") == 0)						return MOVE_SOUND;


	return NO_SOUND;
}


//Functionality =======================
bool j1SoundManager::LoadSoundBlock(const char* xml_folder)
{
	//Load animations data from folders
	//XML load
	LOG("Loading: %s", xml_folder);
	std::string load_folder = xml_folder;
	pugi::xml_document sound_data;
	if (!App->fs->LoadXML(load_folder.c_str(), &sound_data)) return false;

	//Load Animations data
	//Node focused at any unit node
	pugi::xml_node all_node = sound_data.child("SoundAtlas");

	pugi::xml_node type_node;
	//Node focused at any unit action node
	pugi::xml_node sound_node;



	type_node = all_node.first_child();
	while (type_node != NULL)
	{
		Sound_Block* sound_block = new Sound_Block();

		sound_block->SetType(StrToSoundEnum(type_node.attribute("enum").as_string()));
		if (type_node.attribute("song_type").as_string() == "fx") {
			sound_node = type_node.first_child();
			while (sound_node != NULL) {
				sound_block->path = name + "/" + sound_node.attribute("soundPath").as_string();
				load_folder = sound_block->path.c_str();
				sound_block->SetSound(App->audio->LoadFx(load_folder.c_str()) - 1);
				sound_node = sound_node.next_sibling();

			}
			fx_blocks.push_back(sound_block);
		}
		if (type_node.attribute("song_type").as_string() == "music") {
			sound_node = type_node.first_child();
			while (sound_node != NULL) {
				sound_block->path = name + "/" + sound_node.attribute("soundPath").as_string();
				load_folder = sound_block->path.c_str();
				sound_block->SetSound(App->audio->LoadFx(load_folder.c_str()) - 1);
				sound_node = sound_node.next_sibling();

			}
			music_blocks.push_back(sound_block);
		}
		type_node = type_node.next_sibling();


	}
	return true;
}


bool j1SoundManager::PlayFXAudio(SOUND_TYPE sound)
{
	bool ret = false;
	time_t t;
	srand(time(&t));
	Sound_Block* block = nullptr;

	uint size = fx_blocks.size();

	for (uint k = 0; k < size; k++)
	{
		//Pointer to the current block
		block = fx_blocks[k];

		//Compare block unit id
		if (block->GetType() == sound)
		{
			uint rand_sound = rand() % block->GetSoundSize();
			ret=App->audio->PlayFx(block->GetAudio(rand_sound));
			return ret;
		}
	}
	return ret;
}
bool j1SoundManager::PlayMusicAudio(SOUND_TYPE sound)
{
	bool ret = false;
	time_t t;
	srand(time(&t));
	Sound_Block* block = nullptr;

	uint size = music_blocks.size();

	for (uint k = 0; k < size; k++)
	{
		//Pointer to the current block
		block = music_blocks[k];

		//Compare block unit id
		if (block->GetType() == sound)
		{
			uint rand_sound = rand() % block->GetSoundSize();
			ret = App->audio->PlayMusic(block->path.c_str());
			return ret;
		}
	}
	return ret;
}



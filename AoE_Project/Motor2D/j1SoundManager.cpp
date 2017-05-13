#include "j1SoundManager.h"

#include <time.h>
#include <stdlib.h>
#include "j1Render.h"
#include "j1FileSystem.h"
#include "p2Log.h"
#include "j1Audio.h"
#include "j1EntitiesManager.h"
#include "j1App.h"
#include "j1Player.h"

#include "Hud_SelectionPanel.h"


/// Sound Block Class -----------------------
//Constructor =========================
Sound_Block::Sound_Block(SOUND_TYPE type) :type(type)
{

}

//Destructor ==========================
Sound_Block::~Sound_Block()
{
	for (int i=0; i < sound.size(); i++)
	{
		delete sound[i];
	}
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

void Sound_Block::SetSound(const int value, const char* string)
{
	Sound* new_sound = new Sound(value,string);
	
	sound.push_back(new_sound);
}



Sound* Sound_Block::GetAudio(int index)
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
bool j1SoundManager::Enable()
{
	//Load xml sounds ---------------------------
	std::string load_folder = name + "/" + "SoundsData.xml";
	App->sound->LoadSoundBlock(load_folder.c_str());
	// ------------------------------------------
	return true;
}

void j1SoundManager::Disable()
{
	active = false;
	enabled = false;
	CleanUp();
}

bool j1SoundManager::Start()
{
	//timers start
	arrow_sound.Start();
	sword_sound.Start();
	heal_sound.Start();
	alert_sound.Start();

	//Load xml sounds ---------------------------
	std::string load_folder = name + "/" + "SoundsData.xml";
	App->sound->LoadSoundBlock(load_folder.c_str());
	// ------------------------------------------

	return true;
}

bool j1SoundManager::CleanUp()
{
	//Clean the unit blocks
	uint music_size = music_blocks.size();

	for (uint k = 0; k < music_size; k++)
	{
		delete music_blocks[k];
	}
	music_blocks.clear();

	uint fx_size = fx_blocks.size();

	for (uint k = 0; k < fx_size; k++)
	{
		delete fx_blocks[k];
	}
	fx_blocks.clear();

	uint gui_size = gui_blocks.size();

	for (uint k = 0; k < gui_size; k++)
	{
		delete gui_blocks[k];
	}
	gui_blocks.clear();
	return true;
}


//Methods that transform strings to enums (used when loading data from xml)
SOUND_TYPE j1SoundManager::StrToSoundEnum(const char * str) const
{
	//Attack sounds
	if		(strcmp(str, "sword_solo") == 0)						return SWORD_ATTACK_SOUND;
	else if (strcmp(str, "arrow_attack") == 0)						return ARROW_ATTACK_SOUND;
	//Buildings sounds
	else if (strcmp(str, "building_on_fire") == 0)					return BUILDING_ON_FIRE;
	else if (strcmp(str, "barracks_create_select") == 0)			return BARRACK_SOUND;
	else if (strcmp(str, "archery_create_selected") == 0)			return ARCHERY_SOUND;
	else if (strcmp(str, "blacksmith_created_selected") == 0)		return BLACKSMITH_SOUND;
	else if (strcmp(str, "monastery_create_selected") == 0)			return MONASTERY_SOUND;
	else if (strcmp(str, "castle_create_selected") == 0)			return CASTLE_SOUND;
	else if (strcmp(str, "farm_create_selected") == 0)				return FARM_SOUND;
	else if (strcmp(str, "tower_create_selected") == 0)				return TOWER_SOUND;
	else if (strcmp(str, "mill_create_selected") == 0)				return MILL_SOUND;
	else if (strcmp(str, "stable_create_selected") == 0)			return STABLE_SOUND;
	else if (strcmp(str, "university_create_selected") == 0)		return UNIVERSITY_SOUND;
	else if (strcmp(str, "miningcamp_create_selected") == 0)		return MINING_CAMP_SOUND;
	else if (strcmp(str, "lumbercamp_created_selected") == 0)		return LUMBER_CAMP_SOUND;
	else if (strcmp(str, "town_center_select") == 0)				return TOWN_CENTER_SELECTED_SOUND;
	else if (strcmp(str, "villager_create") == 0)					return VILLAGER_CREATED_SOUND;
	else if (strcmp(str, "villager_finish_building") == 0)			return VILLAGER_FINISH_BUILDING;
	else if (strcmp(str, "infantry_archer_horseman_created") == 0)	return INFANTRY_ARCHER_HORSEMAN_CREATED_SOUND;
	//Units Sounds
	else if (strcmp(str, "monk_converting") == 0)					return MONK_CONVERTING_SOUND;
	else if (strcmp(str, "monk_healing") == 0)						return MONK_HEALING_SOUND;
	else if (strcmp(str, "monk_created") == 0)						return MONK_CREATED_SOUND;
	else if (strcmp(str, "horse_death") == 0)						return HORSE_DEATH_SOUND;
	else if (strcmp(str, "horse_selected") == 0)					return HORSE_SELECTED_SOUND;
	else if (strcmp(str, "horse_tasked") == 0)						return HORSE_TASKED_SOUND;
	else if (strcmp(str, "unit_death") == 0)						return DEATH_SOUND;
	else if (strcmp(str, "unit_attack") == 0)						return ATTACK_SOUND;
	else if (strcmp(str, "unit_click") == 0)						return CLICK_SOUND;
	else if (strcmp(str, "unit_move") == 0)							return MOVE_SOUND;
	//Level UP
	else if (strcmp(str, "level_up") == 0)							return LEVEL_UP_SOUND;	
	//Warrior skills
	else if (strcmp(str, "warrior_skill_lvl1_a") == 0)				return WARRIOR_SKILL_LVL1_A;
	else if (strcmp(str, "warrior_skill_lvl1_b") == 0)				return WARRIOR_SKILL_LVL1_B;
	else if (strcmp(str, "warrior_skill_lvl2_a") == 0)				return WARRIOR_SKILL_LVL2_A;
	else if (strcmp(str, "warrior_skill_lvl2_b") == 0)				return WARRIOR_SKILL_LVL2_B;
	//Wizard Skills
	else if (strcmp(str, "wizard_skill_lvl2_a") == 0)				return WIZARD_SKILL_LVL2_A;
	else if (strcmp(str, "wizard_skill_lvl2_b") == 0)				return WIZARD_SKILL_LVL2_B;
	else if (strcmp(str, "wizard_skill_lvl3_a") == 0)				return WIZARD_SKILL_LVL3_A;
	else if (strcmp(str, "wizard_skill_lvl3_b") == 0)				return WIZARD_SKILL_LVL3_B;
	//Hunter skills
	else if (strcmp(str, "hunter_skill_lvl2_a") == 0)				return HUNTER_SKILL_LVL2_A;
	else if (strcmp(str, "hunter_skill_lvl2_b") == 0)				return HUNTER_SKILL_LVL2_B;
	else if (strcmp(str, "hunter_skill_lvl3_a") == 0)				return HUNTER_SKILL_LVL3_A;
	else if (strcmp(str, "hunter_skill_lvl3_b") == 0)				return HUNTER_SKILL_LVL3_B;
	//GUI SOUNDS
	else if (strcmp(str, "main_menu") == 0)							return MAIN_MENU_SONG;
	else if (strcmp(str, "ingame") == 0)							return INGAME_SONG;
	else if (strcmp(str, "win") == 0)								return WIN_SONG;
	else if (strcmp(str, "lost") == 0)								return LOST_SONG;
	else if (strcmp(str, "mainmenu_click") == 0)					return CLICK_MAIN_MENU;
	else if (strcmp(str, "submenu_click") == 0)						return CLICK_SUBMENU;
	else if (strcmp(str, "ingame_click") == 0)						return CLICK_INGAME;
	else if (strcmp(str, "win_fx") == 0)							return WIN_FX;
	else if (strcmp(str, "lost") == 0)								return LOST_FX;
	else if (strcmp(str, "error") == 0)								return ERROR_SOUND;
	else if (strcmp(str, "player_unit_alert") == 0)					return PLAYER_UNIT_ALERT;
	else if (strcmp(str, "player_building_alert") == 0)				return PLAYER_BUILDING_ALERT;


	return NO_SOUND;
}

void j1SoundManager::CheckAttackSound(iPoint location, ATTACK_TYPE attack_type, bool heal)
{
	if (!App->render->IsOnCamera(location))return;

	if (attack_type == MELEE)
	{
		if (sword_sound.Read() > SWORD_SOUND_TIME)
		{
			PlayFXAudio(SWORD_ATTACK_SOUND);
			sword_sound.Start();
		}
	}
	else if (attack_type == DISTANCE)
	{
		if (heal)
		{
			if (heal_sound.Read() > HEAL_SOUND_TIME)
			{
				PlayFXAudio(MONK_HEALING_SOUND);
				heal_sound.Start();
			}
		}
		else 
		{
			if (arrow_sound.Read() > ARROW_SOUND_TIME)
			{
				PlayFXAudio(ARROW_ATTACK_SOUND);
				arrow_sound.Start();
			}
		}
	}
}

void j1SoundManager::CheckAlertSound(iPoint location, bool building)
{
	if (App->render->IsOnCamera(location))return;
	if (alert_sound.Read() > ALERT_SOUND_TIME)
	{
		if(building)
			PlayGUIAudio(PLAYER_BUILDING_ALERT);
		else
			PlayGUIAudio(PLAYER_UNIT_ALERT);
		App->player->selection_panel->SetMinimapAlert(location);
		alert_sound.Start();
	}
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


	std::string path;
	type_node = all_node.first_child();
	while (type_node != NULL)
	{
		Sound_Block* sound_block = new Sound_Block();

		sound_block->SetType(StrToSoundEnum(type_node.attribute("enum").as_string()));
		if (strcmp(type_node.attribute("song_type").as_string(), "fx")==0) {
			sound_node = type_node.first_child();
			while (sound_node != NULL) {
				path = name + "/" + sound_node.attribute("soundPath").as_string();
				load_folder = path.c_str();
				sound_block->SetSound(App->audio->LoadFx(load_folder.c_str()) - 1, path.c_str());
				sound_node = sound_node.next_sibling();

			}
			fx_blocks.push_back(sound_block);
		}
		else if (strcmp(type_node.attribute("song_type").as_string(), "music") == 0) {
			sound_node = type_node.first_child();
			while (sound_node != NULL) {
				path = name + "/" + sound_node.attribute("soundPath").as_string();
				load_folder = path.c_str();
				sound_block->SetSound(-1, path.c_str());
				sound_node = sound_node.next_sibling();

			}
			music_blocks.push_back(sound_block);
		}
		else if (strcmp(type_node.attribute("song_type").as_string(), "gui") == 0) {
			sound_node = type_node.first_child();
			while (sound_node != NULL) {
				path = name + "/" + sound_node.attribute("soundPath").as_string();
				load_folder = path.c_str();
				sound_block->SetSound(App->audio->LoadFx(load_folder.c_str()) - 1, path.c_str());
				sound_node = sound_node.next_sibling();

			}
			gui_blocks.push_back(sound_block);
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
			ret=App->audio->PlayFx(block->GetAudio(rand_sound)->id);
			return ret;
		}
	}
	return ret;
}
bool j1SoundManager::PlayGUIAudio(SOUND_TYPE sound)
{
	bool ret = false;
	time_t t;
	srand(time(&t));
	Sound_Block* block = nullptr;

	uint size = gui_blocks.size();

	for (uint k = 0; k < size; k++)
	{
		//Pointer to the current block
		block = gui_blocks[k];

		//Compare block unit id
		if (block->GetType() == sound)
		{
			uint rand_sound = rand() % block->GetSoundSize();
			ret = App->audio->PlayFx(block->GetAudio(rand_sound)->id);
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
			ret = App->audio->PlayMusic(block->GetAudio(rand_sound)->path.c_str(),0);
			return ret;
		}
	}
	return ret;
}

Sound::Sound(int id, std::string path): id(id),path(path)
{
}

Sound::~Sound()
{
}

#include "j1InputManager.h"

#include "j1App.h"
#include "j1Render.h"
#include "j1Input.h"
#include "p2Log.h"

// Constructors =================================
j1InputManager::j1InputManager()
{
	name = "input_manager";
}

// Destructors ==================================
j1InputManager::~j1InputManager()
{

}


//Game Loop =====================================
// Called when before render is available
bool j1InputManager::Awake(pugi::xml_node& config)
{
	//Node where first key is located
	pugi::xml_node key_node = config.child("key");

	while (key_node != NULL)
	{
		//Get the current key node scancode id
		SDL_Scancode scancode = SDL_SCANCODE_UNKNOWN;
		scancode = SDL_GetScancodeFromName(key_node.attribute("id").as_string());

		if (scancode != SDL_SCANCODE_UNKNOWN)
		{
			//Map pair where new key is allocated
			std::pair<int, INPUT_EVENT> new_key;

			//Build pair with the loaded data
			new_key.first = scancode;
			new_key.second = StrToInputEvent(key_node.attribute("event").as_string());
			if (new_key.second == 0)
			{
				LOG("Error Loading Input Key!");
			}
			else
			{
				this->events_map.insert(new_key);
			}
		}

		//Focus the next key node
		key_node = key_node.next_sibling();
	}

	return true;
}


// Called after all Updates
bool j1InputManager::PostUpdate()
{
	current_events.clear();
	return true;
}

// Called before quitting
bool j1InputManager::CleanUp()
{
	current_events.clear();
	events_map.clear();
	return true;
}


//Functionality ==================================
INPUT_EVENT j1InputManager::StrToInputEvent(const char * str) const
{
	if (strcmp(str, "escape") == 0)				return INPUT_EVENT::ESCAPE;
	if (strcmp(str, "enti_debug") == 0)			return INPUT_EVENT::ENTITY_DEBUG_MODE;
	if (strcmp(str, "map_debug") == 0)			return INPUT_EVENT::MAP_DEBUG_MODE;
	if (strcmp(str, "generate_villager") == 0)	return INPUT_EVENT::GENERATE_VILLAGER;
	if (strcmp(str, "generate_tree") == 0)		return INPUT_EVENT::GENERATE_TREE;
	if (strcmp(str, "generate_bush") == 0)		return INPUT_EVENT::GENERATE_BUSH;
	if (strcmp(str, "generate_stone") == 0)		return INPUT_EVENT::GENERATE_STONE;
	if (strcmp(str, "generate_gold") == 0)		return INPUT_EVENT::GENERATE_GOLD;
	if (strcmp(str, "generate_warrior") == 0)	return INPUT_EVENT::GENERATE_WARRIOR;
	if (strcmp(str, "generate_wizard") == 0)	return INPUT_EVENT::GENERATE_WIZARD;
	if (strcmp(str, "generate_hunter") == 0)	return INPUT_EVENT::GENERATE_HUNTER;
	if (strcmp(str, "set_hability_a") == 0)		return INPUT_EVENT::SET_HABILITY_A;
	if (strcmp(str, "set_hability_b") == 0)		return INPUT_EVENT::SET_HABILITY_B;
	if (strcmp(str, "save") == 0)				return INPUT_EVENT::SAVE;
	if (strcmp(str, "load") == 0)				return INPUT_EVENT::LOAD;
	if (strcmp(str, "pause") == 0)				return INPUT_EVENT::PAUSE;
	if (strcmp(str, "full_screen") == 0)		return INPUT_EVENT::FULL_SCREEN;
	return UNKNOWN_INPUT;
}

void j1InputManager::SendInputEvent(int id, INPUT_STATE state)
{
	//Search the event in the map
	std::map<int, INPUT_EVENT>::const_iterator key_event = events_map.find(id);

	
	while (key_event != events_map.end())
	{

		//If the event is found we add it to the current events map
		if (key_event->first == id)
		{
			std::pair<INPUT_EVENT, INPUT_STATE> key_pressed;
			key_pressed.first = key_event->second;
			key_pressed.second = state;
			current_events.insert(key_pressed);
			break;
		}
		key_event++;
	}
}

INPUT_STATE j1InputManager::GetEvent(INPUT_EVENT _event)
{
	if (!current_events.empty())
	{
		std::map<INPUT_EVENT, INPUT_STATE>::iterator item = current_events.find(_event);
		if (item != current_events.end())
		{
			return item->second;
		}
	}
	return INPUT_NONE;
}
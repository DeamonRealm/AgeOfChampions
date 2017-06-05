#ifndef _J1_INPUT_MANAGER_H_
#define _J1_INPUT_MANAGER_H_

#include "j1Module.h"
#include <map>

enum INPUT_EVENT
{
	UNKNOWN_INPUT = 0,
	
	//Escape
	ESCAPE,

	//Debug
	ENTITY_DEBUG_MODE,
	MAP_DEBUG_MODE,
	
	//Entities generation
	GENERATE_VILLAGER,
	GENERATE_TREE,
	GENERATE_BUSH,
	GENERATE_STONE,
	GENERATE_GOLD,
	
	//Add Player Resources
	ADD_RESOURCES,

	//Champion keys
	GENERATE_WARRIOR,
	GENERATE_WIZARD,
	GENERATE_HUNTER,
	GENERATE_GOD,
	SET_HABILITY_A,
	SET_HABILITY_B,
	SET_HABILITY_C,
	
	//Generate Buildings 
	GENERATE_HOUSE,
	GENERATE_TOWN_CENTER,
	GENERATE_STABLE,
	GENERATE_LUMBER_CAMP,
	GENERATE_MINING_CAMP,
	GENERATE_BLACKSMITH,
	GENERATE_UNIVERSITY,
	GENERATE_MONASTERY,
	GENERATE_ARCHERY_RANGE,
	GENERATE_BARRACK,

	//Pause
	PAUSE,

	//Full Screen
	FULL_SCREEN,

	//Audio Volume
	VOLUME_UP,
	VOLUME_DOWN
};

enum INPUT_STATE
{
	INPUT_NONE = 0,
	INPUT_UP,
	INPUT_DOWN,
	INPUT_REPEAT
};

class j1InputManager : public j1Module
{
public:

	j1InputManager();
	~j1InputManager();

public:

	// Called when before render is available
	bool Awake(pugi::xml_node& );

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

public:
	
	//Functionality -------------------
	//Used when loading input keys
	INPUT_EVENT StrToInputEvent(const char* str)const;
	void SendInputEvent(int id, INPUT_STATE state);
	INPUT_STATE GetEvent(INPUT_EVENT _event);

private:

	//Map with all the input events mapped
	std::multimap<int, INPUT_EVENT> events_map;

	//Events that are happening during this frame
	std::multimap<INPUT_EVENT, INPUT_STATE> current_events;

};
#endif // _J1_INPUT_MANAGER_H_

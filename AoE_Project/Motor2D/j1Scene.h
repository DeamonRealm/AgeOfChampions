#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"

struct SDL_Texture;
class Unit;
class Building;
class Resource;

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	void Init() { active = false; }

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Receive UI input and work with it
	void GUI_Input(UI_Element* target, GUI_INPUT input);

private:

	//Scene Test Entities
	/*
	//Building*	town_center = nullptr;
	Resource*	tree = nullptr;
	Resource*	chop = nullptr;
	Resource*   tree_cut = nullptr;
	Resource*	berry_bush = nullptr;
	Resource*	gold_ore = nullptr;
	Resource*	stone_ore = nullptr;
	*/

	//Scene	UI
	UI_Element* scene_1_screen;

public:

	//Map Folders
	std::vector <std::string> map_folder;

	uint current_map=0;


	//Troops for testing
	Unit*		arbalest = nullptr;

	//Change the current map
	void Change_Map();

	//Load the selected Map
	bool Load_Current_Map();

};

#endif // __j1SCENE_H__
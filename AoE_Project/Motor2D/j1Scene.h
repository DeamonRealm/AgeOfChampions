#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	void Init() { active = false; }

	void Enable();

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

public:

	//Map Folders
	std::vector <std::string> map_folder;

	uint current_map = 0;

	//Change the current map
	void Change_Map();

	//Load the selected Map
	bool Load_Current_Map();

	bool LoadScene();
};

#endif // __j1SCENE_H__
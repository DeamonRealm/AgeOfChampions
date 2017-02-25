#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"

struct SDL_Texture;
struct UI_Button;
struct UI_String;
struct UI_Interactive_String;
struct UI_Image;
struct UI_Element;
struct UI_Text_Box;
struct UI_Scroll;
struct UI_Popup_menu;
class  Unit;
class  Building;

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

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
	Unit*		arbalest = nullptr;
	Building*	town_center = nullptr;

	//Scene	UI
	UI_Element* scene_1_screen;
	
	//pop_menu test
	UI_String*		teutones;
	UI_String*		ingleses;
	UI_String*      japoneses;
	UI_Popup_menu*  civilization_menu;
	UI_Button*		civilization_button;
	UI_String*		selected;

public:

	//Map Folders
	std::vector <std::string> map_folder;

	uint current_map;

	//Change the current map
	void Change_Map();

	//Load the selected Map
	bool Load_Current_Map();

};

#endif // __j1SCENE_H__
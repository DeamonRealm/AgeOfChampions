#ifndef __j1Menu_H__
#define __j1Menu_H__

#include "j1Module.h"

struct SDL_Texture;
class UI_Button;
class UI_String;
class UI_Interactive_String;
class UI_Image;
class UI_Element;
class UI_Text_Box;
class UI_Scroll;
class UI_Popup_menu;
class UI_Fixed_Button;
class  Unit;
class  Building;

class j1Menu : public j1Module
{
public:

	j1Menu();

	// Destructor
	virtual ~j1Menu();

	void Init() { active = true; }

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

	//UI Elements
	UI_Image* main_menu_background;
	UI_Image* standard_background;

	//MainMenu Elements
	UI_Fixed_Button*	exit;
	UI_Fixed_Button*	singleplayer;
	UI_Fixed_Button*	multiplayer;
	UI_Fixed_Button*	turotial;
	UI_Fixed_Button*	mapeditor;
	UI_Fixed_Button*	history;
	UI_Fixed_Button*	options;

	UI_Fixed_Button*	option_selected = nullptr;

	//Menu UI screens
	UI_Element* menu_screen;
	UI_Element* standard_match_screen;

};

#endif // __j1MENU_H__
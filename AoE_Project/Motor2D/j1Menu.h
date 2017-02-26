#ifndef __j1Menu_H__
#define __j1Menu_H__

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
struct UI_Fixed_Button;
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


	//Menu UI screens
	UI_Element* menu_screen;
	UI_Element* standard_match_screen;

};

#endif // __j1MENU_H__
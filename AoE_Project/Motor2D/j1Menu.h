#ifndef __j1Menu_H__
#define __j1Menu_H__

#include "j1Module.h"

class UI_String;
class UI_Image;
class UI_Element;
class UI_Fixed_Button;
class UI_Button;
class  Unit;
class  Building;

class j1Menu : public j1Module
{
public:

	j1Menu();

	// Destructor
	virtual ~j1Menu();

	void Init() { active = true; }
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

	// Receive UI input and work with it
	void GUI_Input(UI_Element* target, GUI_INPUT input);

private:

	//UI Elements
	UI_Image* main_menu_background = nullptr;
	UI_Image* standard_background = nullptr;

	//MainMenu Elements
	UI_Fixed_Button*	exit = nullptr;
	UI_Fixed_Button*	singleplayer = nullptr;
	UI_Fixed_Button*	multiplayer = nullptr;
	UI_Fixed_Button*	turotial = nullptr;
	UI_Fixed_Button*	mapeditor = nullptr;
	UI_Fixed_Button*	history = nullptr;
	UI_Fixed_Button*	options = nullptr;

	UI_Fixed_Button*	option_selected = nullptr;

	//Single Player Buttons
	UI_Button*			standardgame = nullptr;
	UI_Button*			loadgame = nullptr;

	UI_String*			standardgame_text = nullptr;
	UI_String*			loadgame_text = nullptr;


	//Menu UI screens
	UI_Element*			menu_screen = nullptr;
	UI_Element*			standard_match_screen = nullptr;

	UI_String*			wiki = nullptr;
	UI_String*			singleplayer_text = nullptr;

};

#endif // __j1MENU_H__
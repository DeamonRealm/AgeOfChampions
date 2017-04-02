#ifndef HUD_GAME_PANEL
#define HUD_GAME_PANEL

#include "p2Defs.h"
#include <vector>


class UI_String;
class UI_Element;
class UI_Button;
class UI_Image;

enum GUI_INPUT;

enum PLAYER_RESOURCES
{
	GP_WOOD,
	GP_MEAT,
	GP_GOLD,
	GP_STONE,
	GP_NO_RESOURCE
};

#define MAX_GAME_RESOURCES 5
#define MAX_POPULATION 200

class Game_Panel
{
public:
	Game_Panel();
	~Game_Panel();

public:
	// Called before quitting
	bool CleanUp();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool PostUpdate();

	// Draw Game Panel
	bool Draw();

	// Handle_input_GUI
	void Handle_Input(UI_Element* ui_element, GUI_INPUT ui_input);

	// Add Resource
	bool AddResource(int amount, PLAYER_RESOURCES resource_type);

	// Incress and Check Population
	bool IncressPopulation(int amount, bool increase_max = false);
	bool CheckPopulation() const;

	//Get exit Menu
	UI_Element* GetExitMenu();


private:

	// Player Information
	uint			wood;
	uint			meat;
	uint			gold;
	uint			stone;
	uint			population;
	uint			max_population;

	int				wood_width;
	int				meat_width;
	int				gold_width;
	int				stone_width;
	int				population_width;

	std::string		hud_game_text;

	std::vector<UI_String*>		player_resources;

	// Game Menu

	//HUD Panels
	UI_Element*	exit_menu_screen	= nullptr;
	UI_Button*	exit_menu_button	= nullptr;
	UI_Button*	exit_to_main_menu	= nullptr;
	UI_Image*	exit_menu_image		= nullptr;

};



#endif // !HUD_GAME_PANEL

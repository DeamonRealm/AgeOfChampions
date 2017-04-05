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

	// Disable/Enable Module
	void Enable();
	void Disable();

	// Add Resource
	bool AddResource(int amount, PLAYER_RESOURCES resource_type);
	bool UseResource(int amount_wood = 0, int amount_food = 0, int amount_gold = 0, int amount_stone = 0, int used_population = 0);

	// Incress and Check Population
	bool IncressPopulation(int amount, bool increase_max = false);
	bool CheckPopulation() const;

	//Get scores
	int GetAllUnitScore();
	int GetDeathUnitScore();
	int GetResourceScore();

	//Get exit Menu
	UI_Element* GetExitMenu();


	//Scores control
	void IncreaseDeathEnemies();
	void IncreaseDeathAllies();
	void IncreaseAllUnits();
	void IncreaseAllResources();


private:

	// Player Information
	int			wood = 0;
	int			meat = 0;
	int			gold = 0;
	int			stone = 0;
	int			population = 0;
	int			all_resources = 0;
	int			player_death_units = 0;
	int			player_death_enemies = 0;
	int			player_all_units = 0;
	uint		max_population = 0;

	int				wood_width = 0;
	int				meat_width = 0;
	int				gold_width = 0;
	int				stone_width = 0;
	int				population_width = 0;

	std::string		hud_game_text;

	std::vector<UI_String*>		player_resources;
	std::vector<UI_String*>		menu_text;

	// Game Menu

	//HUD Panels
	UI_Element*	exit_menu_screen	= nullptr;
	UI_Button*	exit_menu_button	= nullptr;
	UI_Button*	exit_to_main_menu	= nullptr;
	UI_Button*	cancel_game_menu	= nullptr;
	UI_Image*	exit_menu_image		= nullptr;

};



#endif // !HUD_GAME_PANEL

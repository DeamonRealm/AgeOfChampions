#ifndef HUD_GAME_PANEL
#define HUD_GAME_PANEL

#include "p2Defs.h"
#include <vector>
#include "PugiXml\src\pugixml.hpp"

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
	void Reset();

	bool Load(pugi::xml_node &data);

	bool Save(pugi::xml_node &data) const;

	// Add Resource
	bool AddResource(int amount, PLAYER_RESOURCES resource_type);
	bool UseResource(int amount_wood = 0, int amount_food = 0, int amount_gold = 0, int amount_stone = 0, int used_population = 0, bool use = true);

	// Incress and Check Population
	bool IncressPopulation(int amount, bool increase_max = false);
	bool CheckPopulation() const;

	//Get scores
	int GetAllUnitScore();
	int GetDeathUnitScore();
	int GetResourceScore();

	//Get exit Menu
	UI_Element* GetExitMenu();
	void ActivatePauseMenu();


	//Scores control
	void IncreaseDeathEnemies();
	void IncreaseDeathAllies();
	void IncreaseAllUnits();
	void IncreaseAllResources();

	void DoLose();
	void DoWin();


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
	UI_Button*	save_game_menu_b	= nullptr;
	UI_Button*	load_game_menu_b	= nullptr;
	//Final menu
	UI_Element* final_menu_screen	= nullptr;
	UI_Image*	final_menu_image	= nullptr;
	UI_String*  final_str			= nullptr;
	UI_String*  units_dead			= nullptr;
	UI_String*  your_units			= nullptr;

	UI_String*	resource_info		= nullptr;
	bool		show_resource_info	= false;
	int			curr_resource		= -1;

	bool win = false;
	bool lose = false;
	
	bool game_ended = false;
};



#endif // !HUD_GAME_PANEL

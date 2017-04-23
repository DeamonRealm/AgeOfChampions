#ifndef HUD_ACTION_PANEL
#define HUD_ACTION_PANEL

#include "p2Defs.h"
#include "p2Point.h"
#include <vector>

#include "SDL\include\SDL_rect.h"

#include "Hud_SelectionPanel.h"

#define MAX_PANEL_CELLS 15
#define PANEL_COLUMNS	5
#define CELL_WIDTH		40
#define CELL_HEIGHT		41
#define MAX_SKILLS_LEARNED 6

//class Selection_Panel;
class Game_Panel;

class UI_Image;
class UI_Button;
class UI_Fixed_Button;

class Entity;

enum ENTITY_TYPE;
enum BUILDING_TYPE;
enum UNIT_TYPE;
enum GUI_INPUT;
enum RESEARCH_TECH;

// Basic Action Panel Data -------------------------------------------------------------------------------------

// Cells Data

struct Cells_Data
{
	// Data
	uint		cell_position = 0;
	uint		cell_at_level = 0;
	uint		cell_at_age	  = 0;

	// Data
	SDL_Rect	cell_icon = { 0,0,1,1 };
	int			wood_cost = 0;
	int			food_cost = 0;
	int			gold_cost = 0;
	int			stone_cost = 0;
	int			population_cost = 0;

	// Information
	std::string	cell_info = "";

	UNIT_TYPE	u_type = ((UNIT_TYPE)0);
	BUILDING_TYPE b_type = ((BUILDING_TYPE)0);
	RESEARCH_TECH r_type = ((RESEARCH_TECH)0);

public:
	// Functionality
	void		SetInfo();
	const char*	GetInfo() const;
};

// Base Action Panel ------------------------------------------------------------------------------------------

class Action_Panel_Elements
{
public:
	Action_Panel_Elements();
	~Action_Panel_Elements();
	
	// Reset Panel
	void		 SetDataFromXML();
	void		 UpdateCells();
	virtual void ResetPanel();

	virtual bool ActivateCell(int i);

	virtual void ChangePanelIcons(std::vector<UI_Image*> & actual_panel);

	virtual void ChangePanelTarget(Entity* new_target) { entitis_panel = new_target; };
	Entity*		 GetActualEntity();

	virtual void ChangePlayerGamePanel(Game_Panel* game_panel);

	virtual void LoadPanelFromXML(const pugi::xml_node&	conf);
	virtual void UpgradeCurrentAge(uint curr_age = 1);
	virtual bool UpgradeResearch(RESEARCH_TECH type) { return false; };

	const char*	 GetCellInfo(int i)const;
protected:
	uint						current_age = 2;
	int							cell_lvl[MAX_PANEL_CELLS];

	std::vector<Cells_Data>		panel_icons;
	std::vector<Cells_Data>     cell_data;
	Entity*						entitis_panel = nullptr;
	Game_Panel*					player_game_panel_resources = nullptr;
};

// BUILDINGS ============================================================================================

// TownCenter ------------------------------------------------------------------------------------------

class TownCenterPanel : public Action_Panel_Elements
{
public:
	TownCenterPanel();
	~TownCenterPanel() {};

	// Reset Panel
	void ResetPanel();

	bool ActivateCell(int i);
	void ChampionIsDead(UNIT_TYPE type);

	bool UpgradeResearch(RESEARCH_TECH type);

private:
	bool		got_melechmp = false;
};

// Barrack -----------------------------------------------------------------------------------------------

class BarrackPanel : public Action_Panel_Elements
{
public:
	BarrackPanel();
	~BarrackPanel() {};

	bool UpgradeResearch(RESEARCH_TECH type);

private:

};

// General Units -------------------------------------------------------------------------------------------

class UnitPanel : public Action_Panel_Elements
{
public:
	UnitPanel();
	~UnitPanel() {};

	// Reset Panel
	void ResetPanel();

	bool ActivateCell(int i);

private:

};

// Villager ----------------------------------------------------------------------------------------------

enum VILLAGER_IS_BULIDING
{
	VP_NOT_BUILDING,
	VP_NORMAL,
	VP_MILITARY
};

class VillagerPanel : public Action_Panel_Elements
{
public:
	VillagerPanel();
	~VillagerPanel() {};

	// Pre Update
	void PreUpdate();

	// Reset Panel
	void ResetPanel();

	bool ActivateCell(int i);

	bool Villager_Handle_input(GUI_INPUT input);

	void ChangePanelLvl(int lvl);
	bool GetIsBuilding()const;

private:
	VILLAGER_IS_BULIDING	villagerisbuilding = VP_NOT_BUILDING;

	bool					isbuilding = false;
	Entity*					buildingthis = nullptr;
};

// Hero Panel	-------------------------------------------------------------------------------------------

class HeroPanel : public Action_Panel_Elements
{
public:
	HeroPanel();
	~HeroPanel();

	// Reset Panel
	void ResetPanel();

	bool ActivateCell(int i);
	bool Hero_Handle_input(UI_Element* ui_element, GUI_INPUT ui_input);
	bool Handle_input(GUI_INPUT input);

	void LearnSkill(int i);
	void ChangePanelTarget(Entity* new_target);
	

public:
	UI_Image*						skill_tree = nullptr;
private:

	UNIT_TYPE						champion_selected;

	std::vector<UI_Image*>			skills;
	std::vector<UI_Fixed_Button*>	skills_buttons;

	// Champions Skills;
	std::vector<SDL_Rect>			mele_champion;
	int								activate_skill = -1;
};


// Action Panel --------------------------------------------------------------------------------------------
class Action_Panel
{
public:
	Action_Panel();
	~Action_Panel();

public:
	// Called before quitting
	bool CleanUp();

	// Disable/Enable Panels
	void Enable();
	void Disable();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool PostUpdate();

	// Handle Input
	void Handle_Input(GUI_INPUT newevent);
	// Handle input GUI
	void Handle_Input(UI_Element* ui_element, GUI_INPUT ui_input);
	// Activate with Input
	void ActivateCell(int i);

	// Draw Game Panel
	bool Draw();

	// Set Pointer To Selection Panel
	void SetSelectionPanelPointer(Selection_Panel* selection_panel);

	// Set Pointer To Game Panel
	void SetGamePanelPointer(Game_Panel* game_panel);

	// Panel Settings
	void SetPanelType();
	void SetEntitySelected();
	void SetButtons();

	void DesactivateButtons();

	bool GetOnAction()const;
	bool GetIsIn() const;

	// Check Panel Selected Settings
	void CheckSelected(int size);

	// Return Hero Panel Skil Tree;
	void HeroIsDead(UNIT_TYPE type);

	// Return ActionPanel Screens
	UI_Element* GetActionScreen()const;
	UI_Element* GetHeroSkillTree() const;

	// Upgrade Age
	void UpgradeCivilizationAge(uint curr_age = 2);
	void UpgradeTecnology(RESEARCH_TECH type);


private:

	// HUD Panels Pointer
	Selection_Panel*			player_selection_panel = nullptr;
	Game_Panel*					player_game_panel = nullptr;

	// Action Panel Detection
	bool						isin;
	SDL_Point					mouse_pos;
	SDL_Rect					action_rect;
	UI_Element*					action_screen = nullptr;

	uint						cell_shown = 0;
	UI_String*					cell_information = nullptr;


	// Panell Settings 
	int							cell;
	iPoint						panel_pos;
	std::vector<UI_Image*>		panel_cells;
	std::vector<UI_Button*>		panel_buttons;
	Entity*						actual_entity = nullptr;

	// Entityes Panels
	Action_Panel_Elements*		actualpanel = nullptr;
	
	// Building Panels
	TownCenterPanel*			towncenterpanel = nullptr;
	BarrackPanel*				barrackpanel = nullptr;
	
	// Units Panels
	VillagerPanel*				villagerpanel = nullptr;
	UnitPanel*					unitpanel = nullptr;

	// Unit On Action   
	bool						on_action = false;
	bool						show_cell_info = false;


public:
	// Hero Panel
	HeroPanel*					heropanel = nullptr;


};
#endif // !HUD_ACTION_PANEL
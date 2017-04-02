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

// Base Action Panel ------------------------------------------------------------------------------------------

class Action_Panel_Elements
{
public:
	Action_Panel_Elements();
	~Action_Panel_Elements();
	
	virtual bool ActivateCell(int i) { return false; };

	void AddIcon(SDL_Rect icon_rect, uint position);
	virtual void ChangePanelIcons(std::vector<UI_Image*> & actual_panel) const;

	virtual void ChangePanelTarget(Entity* new_target) { entitis_panel = new_target; };
	Entity* GetActualEntity();

	virtual void ChangePlayerGamePanel(Game_Panel* game_panel);

	void LoadPanel() {};

protected:
	std::vector<SDL_Rect>		panel_icons;

	Entity*						entitis_panel = nullptr;
	
	Game_Panel*					player_game_panel_resources = nullptr;
};

class TownCenterPanel : public Action_Panel_Elements
{
public:
	TownCenterPanel();
	~TownCenterPanel() {};

	bool ActivateCell(int i);

private:
};

// General Units -------------------------------------------------------------------------------------------

class UnitPanel : public Action_Panel_Elements
{
public:
	UnitPanel();
	~UnitPanel() {};

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

	bool ActivateCell(int i);

	void ChangePanelIcons(std::vector<UI_Image*> & actual_panel) const;
	bool Villager_Handle_input(GUI_INPUT input);

	bool GetIsBuilding()const;

private:
	VILLAGER_IS_BULIDING	villagerisbuilding = VP_NOT_BUILDING;

	std::vector<SDL_Rect>	v_normal_panel;
	std::vector<SDL_Rect>	v_militari_panel;

	bool					isbuilding = false;
	Entity*					buildingthis = nullptr;
};

// Hero Panel	-------------------------------------------------------------------------------------------
class HeroPanel : public Action_Panel_Elements
{
public:
	HeroPanel();
	~HeroPanel();

	bool ActivateCell(int i);
	bool Hero_Handle_input(UI_Element* ui_element, GUI_INPUT ui_input);

	void LearnSkill(int i);
	void ChangePanelIcons(std::vector<UI_Image*> & actual_panel) const;
	void ChangePanelTarget(Entity* new_target);
	

public:
	UI_Image*						skill_tree = nullptr;
private:

	UNIT_TYPE						champion_selected;

	std::vector<UI_Image*>			skills;
	std::vector<UI_Fixed_Button*>	skills_buttons;

	// Champions Skills;
	std::vector<SDL_Rect>			mele_champion;
	int								mele_learned[3];
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

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool PostUpdate();

	// Handle Input
	void Handle_Input(GUI_INPUT newevent);

	// Draw Game Panel
	bool Draw();

	// Get Cell Clicked
	int GetCell() const;

	// Get isin bool
	bool GetIsIn() const;

	// Handle_input_GUI
	void Handle_Input(UI_Element* ui_element, GUI_INPUT ui_input);

	// Return Hero Panel Skil Tree;
	UI_Element* GetHeroSkillTree() const;

	// Set Pointer To Selection Panel
	void SetSelectionPanelPointer(Selection_Panel* selection_panel);
	void GetEntitySelected();

	// Set Pointer To Game Panel
	void SetGamePanelPointer(Game_Panel* game_panel);

	// Panel Settings
	void SetPanelType();
	void CheckSelected(int size);

private:
	
	// HUD_Selection Panel Pointer
	Selection_Panel*			player_selection_panel = nullptr;
	Game_Panel*					player_game_panel = nullptr;

	// Action Panel Detection
	bool						isin;
	SDL_Point					mouse_pos;
	SDL_Rect					action_rect;

	// Panell Settings 
	int							cell;
	iPoint						panel_pos;
	std::vector<UI_Image*>		panel_cells;
	Entity*						actual_entity = nullptr;

	// Entityes Panels
	Action_Panel_Elements*		actualpanel = nullptr;
	TownCenterPanel*			towncenter = nullptr;
	UnitPanel*					unitpanel = nullptr;
	VillagerPanel*				villagerpanel = nullptr;


public:
	// Hero Panel
	HeroPanel*					heropanel = nullptr;


};
#endif // !HUD_ACTION_PANEL
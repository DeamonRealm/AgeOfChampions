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

class UI_Image;
class UI_Button;
class Entity;

enum ENTITY_TYPE;
enum BUILDING_TYPE;
enum UNIT_TYPE;
enum GUI_INPUT;


class Action_Panel_Elements
{
public:
	Action_Panel_Elements();
	~Action_Panel_Elements();
	
	virtual bool ActivateCell(int i) { return false; };

	void AddIcon(SDL_Rect icon_rect, uint position);
	void ChangePanelIcons(std::vector<UI_Image*> & actual_panel) const;

	void ChangePanelTarget(Entity* new_target) { entitis_panel = new_target; };
	Entity* GetActualEntity();

	void LoadPanel() {};

protected:
	std::vector<SDL_Rect>		panel_icons;

	Entity*						entitis_panel = nullptr;
};

class TownCenterPanel : public Action_Panel_Elements
{
public:
	TownCenterPanel() : Action_Panel_Elements() 
	{
		panel_icons.reserve(MAX_PANEL_CELLS);
		for (int i = 0; i < MAX_PANEL_CELLS; i++)
		{
			panel_icons[i] = { 0,0,1,1 };
		}
		panel_icons[0] = { 576,585,36,36 };

	};
	~TownCenterPanel() {};

	bool ActivateCell(int i);

private:

};

class UnitPanel : public Action_Panel_Elements
{
public:
	UnitPanel() : Action_Panel_Elements() {
		panel_icons.reserve(MAX_PANEL_CELLS);
		for (int i = 0; i < MAX_PANEL_CELLS; i++)
		{
			panel_icons[i] = { 0,0,1,1 };
		}
		panel_icons[0] = { 0,76,36,36 }; };
	~UnitPanel() {};

	bool ActivateCell(int i);

private:

};

class HeroPanel : public Action_Panel_Elements
{
public:
	HeroPanel();
	~HeroPanel();

	bool ActivateCell(int i);
	void Hero_Handle_input(UI_Element* ui_element, GUI_INPUT ui_input);

public:
	UI_Image*					skill_tree = nullptr;
private:
	
	std::vector<UI_Button*>		skills;

	std::vector<int>			champion_skills_learned;

	// Champions Skills;
	std::vector<SDL_Rect>		mele_champion;
};

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

	// Panel Settings
	void SetPanelType();
	void CheckSelected(int size);

private:
	
	// HUD_Selection Panel Pointer
	Selection_Panel*			player_selection_panel = nullptr;

	bool						isin;

	SDL_Point					mouse_pos;
	SDL_Rect					action_rect;

	// Action 
	int							cell;
	iPoint						panel_pos;
	
	std::vector<UI_Image*>		panel_cells;

	Entity*						actual_entity = nullptr;

	// Panels
	Action_Panel_Elements*		actualpanel = nullptr;

	TownCenterPanel*			towncenter = nullptr;
	UnitPanel*					unitpanel = nullptr;

public:
	HeroPanel*					heropanel = nullptr;


};
#endif // !HUD_ACTION_PANEL
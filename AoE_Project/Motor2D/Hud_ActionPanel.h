#ifndef HUD_ACTION_PANEL
#define HUD_ACTION_PANEL

#include "p2Defs.h"
#include "p2Point.h"
#include <vector>

#include "SDL\include\SDL_rect.h"


#define MAX_PANEL_CELLS 15
#define PANEL_COLUMNS	5
#define CELL_WIDTH		40
#define CELL_HEIGHT		41

class UI_Image;
class Entity;

enum ENTITY_TYPE;
enum GUI_INPUT;

/*
class Action_Panel_Elements
{
public:
	Action_Panel_Elements();
	~Action_Panel_Elements();

private:
	//point 37,624. cell (40x41) w = 200, h = 123

};

Action_Panel_Elements::Action_Panel_Elements()
{
}

Action_Panel_Elements::~Action_Panel_Elements()
{
}
*/
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

	void SetPanelType(Entity* selected);

private:
	
	bool						isin;

	SDL_Point					mouse_pos;
	SDL_Rect					action_rect;

	// Action 
	int							cell;

	iPoint						panel_pos;
	std::vector<UI_Image*>		panel_cells;

};
#endif // !HUD_ACTION_PANEL
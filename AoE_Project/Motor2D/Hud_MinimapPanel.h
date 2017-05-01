#ifndef HUD_MINIMAP_PANEL
#define HUD_MINIMAP_PANEL

#include "p2Defs.h"
#include "p2Point.h"
#include <vector>
#include "PugiXml\src\pugixml.hpp"

#include "SDL\include\SDL_rect.h"

class UI_Element;
class UI_Image;

enum GUI_INPUT;

class Minimap_Panel
{
public:
	Minimap_Panel();
	~Minimap_Panel();

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

	bool Load(pugi::xml_node &data);

	bool Save(pugi::xml_node &data) const;

	// MiniMap Input Funtionality ------------------
	void MoveCameraToPoint(int x = 0, int y = 0);
	void MoveUnitsToPoint(int x = 0, int y = 0);

	// Minimap to Map 
	bool MiniMToMap(int& x, int& y);


public:

	//HUD Panels
	UI_Image*	minimap_background = nullptr;

private:
	// Map Data
	bool		in_minimap = false;
	SDL_Rect    map_rect = { 0,0,0,0 };
	iPoint		map_size;

};

#endif // !HUD_MINIMAP_PANEL

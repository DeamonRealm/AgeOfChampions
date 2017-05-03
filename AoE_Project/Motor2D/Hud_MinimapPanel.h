#ifndef HUD_MINIMAP_PANEL
#define HUD_MINIMAP_PANEL

#include "p2Defs.h"
#include "p2Point.h"
#include <vector>
#include <list>
#include "PugiXml\src\pugixml.hpp"

#include "j1Timer.h"
#include "SDL\include\SDL_rect.h"
#include "SDL_image\include\SDL_image.h"

class UI_Element;
class UI_Image;

enum GUI_INPUT;
enum DIPLOMACY;

#define MINIMAP_UPDATE_RATE		1000
#define MINIMAP_TIME_TO_UPDATE	1
#define MINIMAP_FOW_UPDATE_RATE 500

struct minimap_cell
{
	SDL_Color cell_color = { 255,255,255,255 };
	iPoint    cell_position = { 0,0 };

	bool operator==(const minimap_cell& target)
	{
		if (cell_position == target.cell_position) return true;
		else return false;
	}
};

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
	iPoint MiniMToScreen(int x, int y);

	// Set cells to print
	void SetBuildingToPrint(int x, int y, DIPLOMACY diplomacy_type);
	void RemoveBuildingToPrint(int x, int y, DIPLOMACY diplomacy_type);

	void PushTilestoClear(uint k);
	bool EditMinimapFoW();

public:

	//HUD Panels
	UI_Image*	minimap_background = nullptr;

private:
	// Map Data
	bool		in_minimap = false;
	SDL_Rect    map_rect = { 0,0,0,0 };
	SDL_Rect	minimap_size = { 0,0,0,0 };
	fPoint		half_tile_size = { 0.0f,0.0f };
	iPoint		map_size;

	// Map Cells
	std::vector<minimap_cell>	cells;
	std::vector<minimap_cell>	units_to_print;
	std::list<minimap_cell>		buildings_to_print;

	std::vector<int>	fow_cells_to_clear;

	bool			fow_need_update = false;
	j1Timer			update_timer;
	j1Timer			fow_timer;
	SDL_Surface*	minimap_fow = nullptr;
	SDL_Texture*	minimap_fow_texture = nullptr;

};

#endif // !HUD_MINIMAP_PANEL

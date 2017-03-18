#ifndef HUD_SELECTION_PANEL
#define HUD_SELECTION_PANEL

#include "p2Defs.h"
#include <vector>
#include <list>

#include "j1Timer.h"

#include "SDL\include\SDL_rect.h"

class UI_String;
class UI_Image;

class Entity;
class Unit;
class Building;
class Resource;

enum ENTITY_TYPE;
enum GUI_INPUT;


enum SELECT_TYPE
{
	SINGLE,
	GROUP,
	DOUBLECLICK,
	ADD
};

class Entity_Profile
{
public:
	Entity_Profile();
	~Entity_Profile();

	void SetEntity(Entity* entity_selected);
	Entity* GetEntity();
	void DrawProfile() const;
	void UpdateStats();

private:

	std::string		profile_text;

	Entity*			element = nullptr;
	ENTITY_TYPE		e_type;

	//General
	UI_String*		name;
	//UI_String*		civilizacion;
	UI_String*		diplomacy;
	bool			isenemy;

	int				life_update;
	int				m_life;
	UI_String*		life;

	//Units and Offensive Buildings
	uint			u_attack;
	UI_String*		attack;
	uint			u_deffence;
	UI_String*		deffence;

	uint			u_range;
	UI_String*		range;

	//Buildings
	uint			u_capacity;
	uint			m_capacity;
	UI_String*		capacity;

	//Resources
	uint			u_resources;
	UI_String*		resources;
};


class Selection_Panel
{
public:
	Selection_Panel();
	~Selection_Panel();

public:
	// Called before all Updates
	bool PreUpdate();

	// Called before quitting
	bool CleanUp();

	// Handle Input
	void Handle_Input(GUI_INPUT newevent);

	// Draw Selection Panel
	bool Draw();
	void DrawGroup();

	// Coordinates Methods
	bool UnitisIn(int x, int y, int width, int height);
	bool PointisIn(int x, int y) const;
	bool PointisInViewport(int x = 0, int y = 0) ;

	// Select Methods
	void Select(SELECT_TYPE type);

	// Clear Selected Elements List
	void UnSelect_Entity();

	void Expand_SelectionRect();
	Entity*	GetUpperEntity(int x, int y)const;
	 
	// Selction Panel (Group Functions)
	void SetGroupProfile();

private:
	// Mouse_pos
	int mouse_x = 0;
	int mouse_y = 0;

	// Group selection
	uint			max_selected_units = 60;
	uint			max_row_units = 16;
	uint			row_size = 608;

	j1Timer			double_click;
	bool			double_clickon;

	bool			expand;
	SDL_Rect		selection_rect;
	SDL_Rect		map_viewport;


	std::list<Entity*>	selected_elements;
	std::vector<UI_Image*>  group_profile;

	Entity_Profile*		Selected = nullptr;
	
	// Used for mouse detection
	Entity*			UpperEntity = nullptr;

	
};

#endif // !HUD_SELECTION_PANEL


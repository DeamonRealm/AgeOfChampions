#ifndef HUD_SELECTION_PANEL
#define HUD_SELECTION_PANEL

#include "p2Defs.h"
#include <vector>
#include <list>

#include "j1Timer.h"

#include "SDL\include\SDL_rect.h"

#include "Quadtree.h"

//Units/Bukdings Actions
#include "j1ActionManager.h"

#define OFFSET_Y 10
#define OFFSET_X 10

#define REFRESH_RATE 200

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

	void Reset();

	void DrawProfile() const;
	void UpdateStats();

private:

	std::string		profile_text;

	Entity*			element = nullptr;
	ENTITY_TYPE		e_type;
	SDL_Rect		background = {0,0,0,0};

	//General
	UI_String*		name = nullptr;
	//UI_String*		civilization;
	UI_String*		diplomacy = nullptr;
	bool			isenemy = false;

	int				life_update = 0;
	int				m_life = 0;
	UI_String*		life = nullptr;

	//Units and Offensive Buildings
	uint			u_attack = 0;
	UI_String*		attack = nullptr;
	uint			u_deffence = 0;
	UI_String*		deffence = nullptr;

	uint			u_range = 0;
	UI_String*		range = nullptr;

	//Buildings
	uint			u_capacity = 0;
	uint			m_capacity = 0;
	UI_String*		capacity = nullptr;

	//Resources
	int				u_resources = 0;
	UI_String*		resources = nullptr;

	//Buff stats
	int				attack_up = 0;
	int				deffence_up = 0;
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

	// GUI Handle Input
	void Handle_Input(UI_Element* target, GUI_INPUT input);

	// Disable/Enable Module
	void Enable();
	void Disable();
	void Reset();

	bool Load(pugi::xml_node &data);

	bool Save(pugi::xml_node &data) const;

	// Draw Selection Panel
	bool Draw();
	void DrawGroup();

	// Coordinates Methods
	bool PointisInViewport(int x = 0, int y = 0) ;

	// Move Methods
	void MoveSelectedToPoint(int x, int y);

	// Select Methods
	void Select(SELECT_TYPE type);
	void UpdateSelected();
	bool UpdateGroup();

	// Clear Selected Elements List
	void UnSelect_Entity();

	void Expand_SelectionRect();
	Entity*	GetUpperEntity(int x, int y);
	 
	// Selection Panel (Group Functions)
	void SetGroupProfile();

	Entity* GetSelected() const;
	uint	GetSelectedSize() const;

	void    GetSelectedType(DIPLOMACY &d_type, ENTITY_TYPE &e_type, UNIT_TYPE &u_type, BUILDING_TYPE &b_type);
	void	GetChampionsSelected(Unit* &warrior, Unit* &wizard, Unit* &archer);
	bool    GetSelectedIsEntity();
	void	ResetSelectedType(SELECT_TYPE select_type);
	void	ResetSelectedType();
	void	CheckSelectedType();
	void	RemoveDeadSelectedUnits(Unit*);

	UI_Element*	GetViewport();
	bool	GetInViewport()const;

private:

	bool WindowsMove();
	// SelectionPanel Viewport
	UI_Element*		viewport = nullptr;
	bool			inviewport = false;

	// Mouse_pos
	int mouse_x = 0;
	int mouse_y = 0;

	// Group selection
	uint			max_selected_units = 60;
	uint			max_row_units = 16;
	uint			row_size = 608;

	j1Timer			double_click;
	bool			double_clickon = false;

	bool			expand = false;
	SDL_Rect		selection_rect;
	const SDL_Rect	map_viewport;


	std::list<Entity*>		selected_elements;
	std::vector<UI_Image*>  group_profile;

	std::vector<Unit*>		unit_quad_selection;
	std::vector<Building*>  building_quad_selection;
	std::vector<Resource*>	resource_quad_selection;

	Entity_Profile*		Selected = nullptr;
	
	// Used for mouse detection
	Entity*			UpperEntity = nullptr;
	j1Timer			refresh_upperentity;

	// Selected Types;
	DIPLOMACY		selected_diplomacy;
	ENTITY_TYPE		selected_entity_type;
	UNIT_TYPE		selected_unit_type;
	BUILDING_TYPE   selected_building_type;
	bool			champions_selected = false;

	//Action inputs
	Action* action_command = nullptr;
	
};

#endif // !HUD_SELECTION_PANEL


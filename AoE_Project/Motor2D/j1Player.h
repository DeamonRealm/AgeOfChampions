#ifndef _J1PLAYER_H_
#define _J1PLAYER_H_

#include "j1Module.h"
#include "j1Timer.h"
#include "p2Point.h"
#include "SDL\include\SDL_rect.h"


struct SDL_Texture;


class Entity;
class Unit;
class Resource;
class Building;
class UI_String;
class UI_Image;

class Entity_Profile;
class Selection_Panel;
class Game_Panel;



enum SELECT_TYPE
{
	SINGLE,
	GROUP,
	DOUBLECLICK,
	ADD
};

enum ACTION
{
	LCLICK,
	RCLICK
};
//Animator Module -------------------------------
class j1Player : public j1Module
{
public:

	j1Player();
	~j1Player();

public:

	void Init() { active = false; }

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

private:

	std::string		civilization;
	
	//group selection
	uint			max_selected_units = 60;
	uint			max_row_units = 16;
	uint			row_size = 608;

	j1Timer			double_click;
	bool			double_clickon;

	SDL_Rect		selection_rect;

	UI_Image*			game_hud = nullptr;

	std::list<Unit*>	actual_population;
	std::list<Entity*>	selected_elements;

	std::vector<UI_Image*>  group_profile;

	//Selection_Panel*		SelectionPanel;
	
	//Resources test
	std::list<Entity*> game_entityes;

	//Used for mouse detection
	Entity*			UpperEntity = nullptr;


	// Just for testing Recolection;
	Resource*	tree = nullptr;
	Resource*	chop = nullptr;
	Resource*   tree_cut = nullptr;
	Resource*	berry_bush = nullptr;
	Resource*	gold_ore = nullptr;
	Resource*	stone_ore = nullptr;

	//HUD Panels
	Entity_Profile*		Selected = nullptr;
	//Selection_Panel*		SelectionPanel;
	Game_Panel*			game_panel = nullptr;


public:

	//Functionality -------------------

	//Draw Profiles 
	void DrawGroup();

	//Move Method
	bool MoveSelectedUnits(int x, int y);
	
	//Coordinates Methods
	bool UnitisIn(int x, int y, int width, int height);
	bool PointisIn(int x, int y) const;

	//Select Methods
	void Select(SELECT_TYPE type);
	void UnSelect_Entity();
	void Expand_SelectionRect();
	Entity*	GetUpperEntity(int x, int y)const;

	//Selction Panel (Group Functions)
	void SetGroupProfile();

	//Action Panel
	bool DoAction(ACTION type);

};
#endif // !_J1PLAYER_H_

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

class Entity_Profile
{
public:
	Entity_Profile();
	~Entity_Profile();

	void SetEntity(Entity* entity_selected);
	Entity* GetEntity();
	void DrawProfile() const;

private:

	Entity*			element = nullptr;

	UI_String*		name;
	UI_String*		diplomacy;


};

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
	uint			wood = 0;
	uint			meat = 0;
	uint			gold = 0;
	uint			stone = 0;
	uint			population = 0;
	uint			max_population = 5;
	
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

	Entity_Profile* Selected;
	
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

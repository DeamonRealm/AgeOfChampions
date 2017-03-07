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
	void DrawProfile() const;

private:

	Entity*			element;

	UI_String*		name;
	UI_String*		diplomacy;


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

	j1Timer			double_click;
	bool			double_clickon;

	SDL_Rect		selection_rect;

	UI_Image*			game_hud;

	std::list<Unit*>	actual_population;
	std::list<Entity*>	selected_elements;

	Entity_Profile* Selected;


public:

	//Functionality -------------------

	//Draw Profiles 
	void DrawGroup();
	
	bool UnitisIn(int x, int y, int width, int height);
	bool PointisIn(int x, int y);

	void Select_Entity();
	void Select_Group();
	void Select_Type();

	void UnSelect_Entity();

	void Expand_SelectionRect();
};
#endif // !_J1PLAYER_H_

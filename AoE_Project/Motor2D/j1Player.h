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
class UI_Image;


struct Profile
{
	Entity*			entity_profile;
	SDL_Texture*	texture = nullptr;

	void DrawBasicAt(int x = 0, int y = 0);
	void DrawProfileAt(int x = 0, int y = 0);
	
	bool MouseisIn(int x = 0, int y = 0);
};

//Animator Module -------------------------------
class j1Player : public j1Module
{
public:

	j1Player();
	~j1Player();

public:

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
	uint			Stone = 0;
	uint			population = 0;
	uint			max_population = 5;

	std::list<Unit*>	actual_population;
	std::list<Entity*>	selected_elements;


public:

	//Functionality -------------------
	
	/*void Generate_Unit();
	void Generate_Building();

	void select_entity();

	//Draw Profiles 
	void DrawUnitsSelected();
	void DrawEntitysSelected();
	*/

	void Select_Entity();
	void CheckPixelAlpha(int x, int y);
};
#endif // !_J1PLAYER_H_

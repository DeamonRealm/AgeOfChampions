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
class Action_Panel;


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


	//Enable and disable modules
	virtual void Enable();

	virtual void Disable();

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

	// GUI_INPUT
	void GUI_Input(UI_Element* target, GUI_INPUT input);

private:

	std::string				civilization;

	UI_Image*				game_hud = nullptr;

	std::vector<Unit*>		upgraded_units;

public: /*This is temporal*/

	//HUD Panels
	Selection_Panel*		selection_panel = nullptr;
	Game_Panel*				game_panel = nullptr;
	Action_Panel*			action_panel = nullptr;

public:

	//Functionality -------------------

};
#endif // !_J1PLAYER_H_

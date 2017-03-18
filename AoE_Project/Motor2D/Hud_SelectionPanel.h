#ifndef HUD_SELECTION_PANEL
#define HUD_SELECTION_PANEL

#include "p2Defs.h"
#include <vector>
#include <list>

class UI_String;

class Entity;
class Unit;
class Building;
class Resource;

enum ENTITY_TYPE;

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
	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Draw Selection Panel
	bool Draw();

private:
	
};

#endif // !HUD_SELECTION_PANEL


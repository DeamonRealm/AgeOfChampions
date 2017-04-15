#ifndef _ENTITIES_MANAGER_
#define _ENTITIES_MANAGER_

#include "j1Module.h"
#include "Quadtree.h"

#include "BaseEntities.h"
#include "Units.h"
#include "Buildings.h"
#include "Champions.h"

class Action;
class ActionManager;

enum RESEARCH_TECH;

struct LessDistance
{
	bool operator()(const Unit* l, const Unit* r)
	{
		return l->distance_to_target >= r->distance_to_target;
	}
};

class j1EntitiesManager : public j1Module
{
public:

	j1EntitiesManager();
	~j1EntitiesManager();

public:

	// Called before render is available
	bool Awake(pugi::xml_node& config_node);

	void Disable();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called each loop iteration
	virtual bool PostUpdate();

	//Draw entities
	bool Draw()const;

	// Called before quitting
	bool CleanUp();

	//Handle Console Input ----------------------
	void Console_Command_Input(Command* command, Cvar* cvar, std::string* input);
	void Console_Cvar_Input(Cvar* cvar, Command* command_type, std::string* input);

private:

	//Lists of current game entities
	std::list<Unit*>		units;
	std::list<Unit*>		death_units;
	std::list<Resource*>	resources;
	std::list<Building*>	buildings;


	//Vector where all the killed/destroyed entities are placed
	std::vector<Entity*>	wasted_units;

	//Vector of predefined units
	std::vector<Unit*>		units_defs;
	std::vector<Resource*>	resources_defs;
	std::vector<Building*>	buildings_defs;

	// Cvar that defines the console unit generator unit type
	Cvar* unit_cvar;

	//Methods to add entities definitions
	bool		AddUnitDefinition(const pugi::xml_node* unit_node);
	bool		AddResourceDefinition(const pugi::xml_node* resource_node);
	bool		AddBuildingDefinition(const pugi::xml_node* building_node);

	//Check if the entity civilizations string contains the chosen one
	bool		CivilizationCheck(char* civs_str, const char* chosen_civ);

public:

	//Entities Quad Trees to help other modules work with enetities
	m_QuadTree<Unit*>			units_quadtree;
	m_QuadTree<Resource*>		resources_quadtree;
	m_QuadTree<Building*>		buildings_quadtree;

	//Functionality -------------------
	//Load all chosen civilization data
	bool		LoadCivilization(const char* folder);
	//Factory Methods -------
	Unit*		GenerateUnit(UNIT_TYPE type, DIPLOMACY diplomacy, bool push_in_list = true);
	Building*	GenerateBuilding(BUILDING_TYPE type, DIPLOMACY diplomacy, bool push_in_list = true);
	Resource*	GenerateResource(RESOURCE_TYPE type);

	//Get Methods -----------
	const std::list<Unit*>*		UnitsList()const;
	const std::list<Building*>*	BuildingList()const;
	const std::list<Resource*>*	ResourceList()const;

	//Delete Methods --------
	bool		DeleteEntity(Entity* entity);

	//Entities Management Methods
	bool		SetUnitPath(Unit* target, const iPoint& goal);
	bool		SetGroupPath(const std::vector<Unit*>& targets, const iPoint& goal);

	//Units lists movement units used in buildings cover
	void		AddUnit(Unit* unit);
	void		AddDeathUnit(Unit* unit);
	void		RemoveDeathUnit(Unit* unit);

	Unit*		PopUnit(const Unit* unit);

	//Get the nearest resource save point from the coordinate
	Building* SearchNearestSavePoint(const iPoint& point);

	//Organize unit vector
	std::priority_queue<Unit*, std::vector<Unit*>, LessDistance > OrganizeByNearest(std::vector<Unit*>& vec, Circle& target);

	//Upgrade Entity
	void UpgradeEntity(RESEARCH_TECH type, DIPLOMACY diplomacy);
	bool UpgradeUnit(std::vector<Unit*> &upgraded, ENTITY_TYPE e_type = NO_ENTITY, UNIT_TYPE u_type = NO_UNIT, UNIT_TYPE new_type = NO_UNIT, DIPLOMACY e_diplomacy = ALLY);
	
	

};
#endif // _ENTITIES_MANAGER_

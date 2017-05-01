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

	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&)const;

public:

	//Lists of current game entities
	std::list<Unit*>		units;
	std::list<Resource*>	resources;
	std::list<Building*>	buildings;

private:

	std::list<Unit*>		death_units;
	std::list<Building*>	death_buildings;
	std::list<Champion*>	champions_blue;
	std::list<Champion*>	champions_red;


	//Vector where all the killed/destroyed entities are placed
	std::vector<Entity*>	wasted_units;

	//Vectors of predefined units
	std::vector<Unit*>		ally_units_defs;
	std::vector<Unit*>		enemy_units_defs;
	std::vector<Building*>	buildings_defs;
	std::vector<Resource*>	resources_defs;

	//Methods to add entities definitions
	bool		AddUnitDefinition(const pugi::xml_node* unit_node);
	bool		AddResourceDefinition(const pugi::xml_node* resource_node);
	bool		AddBuildingDefinition(const pugi::xml_node* building_node);

	//Check if the entity civilizations string contains the chosen one
	bool		CivilizationCheck(char* civs_str, const char* chosen_civ);

public:
	void GetChampion(Champion* champion, DIPLOMACY diplomacy);
	void ExtractChampion(Champion* champion, DIPLOMACY diplomacy);

	//Send Experience to champions
	void GetExperienceFromUnit(int exp,DIPLOMACY diplomacy);
	//Entities Quad Trees to help other modules work with entities
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
	std::list<Unit*>	GetDeathUnitList();
	void				AddUnit(Unit* unit);
	void				AddDeathUnit(Unit* unit);
	void				ResurrectUnit(Unit* unit);
	void				RemoveDeathUnit(Unit* unit);

	Unit*				PopUnit(const Unit* unit);

	void				AddDeadBuilding(Building* build);
	void				RemoveDeathBuilding(Building* build);


	//Organize unit vector
	std::priority_queue<Unit*, std::vector<Unit*>, LessDistance > OrganizeByNearest(std::vector<Unit*>& vec, Circle& target);

	//Upgrade Entity
	void UpgradeEntity(RESEARCH_TECH type, DIPLOMACY diplomacy);
	bool UpgradeUnit(UNIT_TYPE u_type = NO_UNIT, UNIT_TYPE new_type = NO_UNIT, DIPLOMACY e_diplomacy = ALLY);
	
	
	//Get nearest Entity methods
	Unit*		GetNearestUnit(iPoint point, UNIT_TYPE type);
	Building*	GetNearestBuilding(iPoint point, BUILDING_TYPE type);
	Resource*	GetNearestResource(iPoint point, RESOURCE_TYPE type);

};
#endif // _ENTITIES_MANAGER_

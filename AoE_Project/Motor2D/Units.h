#ifndef __UNITS_H__
#define __UNITS_H__

#include "BaseEntities.h"
#include "Hud_GamePanel.h"

///Class Villager -----------
class Villager : public Unit
{
public:

	Villager();
	Villager(const Villager& copy);
	~Villager();
	
private:

	//Item used id
	ITEM_TYPE item_type = NO_ITEM;

	//Resource related data
	PLAYER_RESOURCES resource_collected_type = GP_NO_RESOURCE;
	uint resources_capacity = 0;
	uint current_resources = 0;
	uint recollect_capacity = 0;
	uint stone_recollect_rate = 0;
	uint gold_recollect_rate = 0;
	uint tree_recollect_rate = 0;
	uint bush_recollect_rate = 0;

public:

	//Functionality ----
	//Save/Load methods ----
	void	SaveAsDef(pugi::xml_node& node);

	//Actions -----
	bool Die();

	//Get Methods -
	ITEM_TYPE			GetItemType()const;
	PLAYER_RESOURCES	GetResourceCollectedType()const;
	uint				GetCurrentResources()const;
	uint				GetRecollectCapacity()const;
	uint				GetResourcesCapacity()const;
	uint				GetRecollectRate(RESOURCE_TYPE type)const;

	//Set Methods -
	void SetItemType(ITEM_TYPE type);
	void SetResourceCollectedType(PLAYER_RESOURCES type);
	void SetResourcesCapacity(uint value);
	void SetRecollectCapacity(uint value);
	void SetRecollectRate(uint value, RESOURCE_TYPE res_type);
	void SetCurrentResources(uint value);

	//Actions -----
	bool Recollect(Resource* target);
	bool SaveResources(Building* save_point);
	void CheckRecollectResource(RESOURCE_TYPE type);
	void CheckCarryResource();
	void ResetResourcesData();
};
/// -------------------------
#endif // !__UNITS_H__


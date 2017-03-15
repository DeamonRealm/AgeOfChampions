#ifndef __UNITS_H__
#define __UNITS_H__

#include "BaseEntities.h"

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
	uint resources_capacity = 0;
	uint current_resources = 0;
	uint recollect_capacity = 0;
	uint recollect_rate = 0;

public:

	//Game Loop --------
	bool Update();

	//Functionality ----
	//Get Methods -
	ITEM_TYPE GetItemType()const;

	//Set Methods -
	void SetItemType(ITEM_TYPE type);
	void SetResourcesCapacity(uint value);
	void SetRecollectCapacity(uint value);
	void SetRecollectRate(uint value);

	//Actions -----
	bool Interact();
	bool Recollect();

};
/// -------------------------
#endif // !__UNITS_H__


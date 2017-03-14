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

	//Resource related data
	uint resources_capacity = 0;
	uint current_resources = 0;
	uint recollect_capacity = 0;
	uint recollect_rate = 0;

public:

	//Game Loop ---
	bool Update();

	//Actions -----
	bool Interact();
	bool Recollect();

};
/// -------------------------
#endif // !__UNITS_H__


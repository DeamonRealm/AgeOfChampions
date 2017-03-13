#ifndef __VILLAGER_H__
#define __VILLAGER_H__

#include "BaseEntities.h"

///Class Villager -----------
class Villager : public Unit
{
public:

	Villager();
	~Villager();
	
private:

	uint extraction_capacity = 0;

public:

	//Game Loop ---
	bool Update();

	//Actions -----
	bool Interact();
	bool Recollect();

};
#endif // !__VILLAGER_H__


#ifndef _RESOURCES_H_
#define _RESOURCES_H_

#include "BaseEntities.h"

///Class Tree ---------------
class Tree : public Resource
{
public: 

	Tree();
	Tree(const Tree& copy);
	~Tree();

private:

	uint cortex = 0;

public:

	//Functionality ---------
	bool	ExtractResources(uint* value);
	uint	GetLife()const;

};
/// -------------------------

#endif // !_RESOURCES_H_

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
	//Save/Load methods ----
	void	SaveAsDef(pugi::xml_node& node);
	void	LoadAsDef(pugi::xml_node& node);

	//Draw ------------------
	bool	Draw(bool debug);

	//Get Methods ----------
	uint	GetCortex()const;

	//Set Methods ----------
	void	SetCortex(uint val);

	//Extract ---------------
	bool	ExtractResources(uint* value);

};
/// -------------------------

#endif // _RESOURCES_H_

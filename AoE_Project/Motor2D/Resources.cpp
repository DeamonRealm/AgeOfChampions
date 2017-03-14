#include "Resources.h"

#include "j1App.h"
#include "j1EntitiesManager.h"
#include "j1Animator.h"

///Class Tree -----------------------------------
//Constructors ========================
Tree::Tree() :Resource()
{

}

Tree::Tree(const Tree & copy) : Resource(copy), cortex(copy.cortex)
{

}

//Destructors =========================
Tree::~Tree()
{

}

bool Tree::ExtractResources(uint * value)
{
	if (life == 0)return false;
	//Check the tree cortex
	else if (cortex > 0)
	{
		cortex -= MIN(cortex, *value);
		if (cortex == 0)
		{
			resource_type = TREE_CUT;
			App->animator->ResourcePlay(this);
		}
	}

	//Check the tree resources
	else if (life <= *value)
	{
		*value = life;
		life = 0;

		//Now the tree is only a static chop that can't interact 
		resource_type = CHOP;
		App->animator->ResourcePlay(this);
		return true;
	}
	else
	{
		life -= *value;
	}

	return true;
}

uint Tree::GetLife() const
{
	if (cortex > 0)return cortex;
	return life;
}

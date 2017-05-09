#ifndef __j1MODULE_H__
#define __j1MODULE_H__

#include "p2Defs.h"
#include <vector>
#include <list>
#include "PugiXml\src\pugixml.hpp"

class j1App;
class UI_Element;
enum GUI_INPUT;

class j1Module
{
public:

	j1Module() : active(false)
	{}

	virtual ~j1Module()
	{}

	virtual void Init()
	{
		active = true;
		enabled = true;
	}

	//Enable and disable modules
	void Active()
	{
		active = true;
	}
	virtual bool Enable()
	{
		active = true;
		return true;
	}

	virtual void Disable()
	{
		active = false;
		enabled = false;
	}

	virtual void Reset()
	{

	}

	// Called before render is available
	virtual bool Awake(pugi::xml_node&)
	{
		return true;
	}

	// Called before the first frame
	virtual bool Start()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PreUpdate()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool Update(float dt)
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PostUpdate()
	{
		return true;
	}

	// Called before quitting
	virtual bool CleanUp()
	{
		return true;
	}

	virtual bool Load(pugi::xml_node&)
	{
		return true;
	}

	virtual bool Save(pugi::xml_node&) const
	{
		return true;
	}

	//GUI functions -------------------
	virtual void GUI_Input(UI_Element* target, GUI_INPUT input)
	{

	}

public:

	std::string	name;
	bool		active = false;
	bool		enabled = false;

};

#endif // __j1MODULE_H__
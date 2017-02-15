#ifndef _C_VAR_
#define _C_VAR_

#include <stdlib.h>
#include "j1Module.h"

enum C_VAR_TYPE {

	UNDEF,
	INT_VAR,
	FLOAT_VAR,
	CHAR_VAR,
	BOOLEAN_VAR
};

class Cvar {
public:

	Cvar(const char* name, const char* description, char* value, C_VAR_TYPE var_type,j1Module* module_var, bool only_read) :name(name), description(description), value(value), var_type(var_type), module_var(module_var), only_read(only_read) {}

	Cvar() {};

	~Cvar()
	{
		module_var = nullptr;
	}

private:

	std::string	name;
	std::string	description;

	std::string	value;
	C_VAR_TYPE	var_type;

	bool only_read;

	j1Module*	module_var;

public:

	//Cvar general data functions -------------------------
	//Get cvar name
	const  std::string* GetCvarName()const
	{
		return &name;
	}
	//Get cvar description
	const  std::string* GetCvarDescription()const
	{
		return &description;
	}
	//Get cvar type
	C_VAR_TYPE GetCvarType()const
	{
		return var_type;
	}
	//Get cvar module
	j1Module*	GetCvarModule()const
	{
		return module_var;
	}


	//Cvar value functions --------------------------------
	double	GetValueAsNum()const
	{
		return atoi(value.c_str());
	}
	
	bool GetValueAsBool()const
	{
		return (value == "true" || value == "t");
	}

	//Get cvar value string
	const  std::string* GetValueString()const
	{
		return &value;
	}
	//Set cvar value
	void SetValue(const char* new_value)
	{
		if (strlen(new_value) > 0)
		{
			value.clear();
			value += new_value;
		}

	}

	//Get editable state
	bool GetOnlyReadState()const
	{
		return only_read;
	}

};
#endif

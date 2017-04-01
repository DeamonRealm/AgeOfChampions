#ifndef _J1_AI_H_
#define _J1_AI_H_

#include "j1Module.h"
#include "p2Log.h"


class j1AI : public j1Module
{
public:
	j1AI();
	~j1AI();

public:
	void Init() { active = false; };

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();


	//Called each loop iteration
	bool Update();


	// Called before quitting
	bool CleanUp();

private:


};


#endif // !_J1_AI_H_


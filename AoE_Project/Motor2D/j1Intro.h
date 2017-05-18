#ifndef _J1INTRO_
#define _J1INTRO_

#include "j1Module.h"
#include "j1App.h"
#include "j1Video.h"

/// Intro Scene ---------------------------------
class j1Intro : public j1Module
{
public:

	j1Intro()
	{

	}

	~j1Intro()
	{

	}

	void Init()
	{
		active = true;
		enabled = false;
	}

	bool Start()
	{
		enabled = active = true;
		App->video->PlayVideo("data/intro_video.ogv", { 100,100,1166,568 });
		return true;
	}

	bool Update(float dt)
	{
		if (!App->video->want_to_play || App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			this->Disable();
			App->video->Disable();
			App->menu->Active();
			App->EnableActiveModules();
		}
		return true;
	}

private:


public:

};
/// ---------------------------------------------
#endif
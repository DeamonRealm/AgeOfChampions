#include "j1Player.h"

#include "p2Log.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Animator.h"

#include "j1App.h"
#include "j1Gui.h"
#include "j1EntitiesManager.h"


void Profile::DrawBasicAt(int x, int y)
{
}

void Profile::DrawProfileAt(int x, int y)
{
}

bool Profile::MouseisIn(int x, int y)
{
	return false;
}

j1Player::j1Player()
{
	name = "player";
}

j1Player::~j1Player()
{
}

bool j1Player::Awake(pugi::xml_node& config)
{
	LOG("Loading Player");
	bool ret = true;


	return ret;
}

bool j1Player::Start()
{
	return true;
}

bool j1Player::PreUpdate()
{
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		Select_Entity();
	}

	if(App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
	{
		int x, y;
		App->input->GetMousePosition(x, y);
		Unit* new_unit = App->entities_manager->GenerateUnit(MILITIA);
		new_unit->SetPosition(x - App->render->camera.x, y - App->render->camera.y);
		actual_population.push_back(new_unit);
	}
	if (App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN)
	{
		int x, y;
		App->input->GetMousePosition(x, y);
		Unit* new_unit = App->entities_manager->GenerateUnit(ARBALEST);
		new_unit->SetPosition(x - App->render->camera.x, y - App->render->camera.y);
		actual_population.push_back(new_unit);
	}
	return true;
}

bool j1Player::PostUpdate()
{
	return true;
}

bool j1Player::CleanUp()
{
	actual_population.clear();
	return true;
}

void j1Player::Select_Entity()
{
	int x, y;
	App->input->GetMousePosition(x, y);
	x -= App->render->camera.x;
	y -= App->render->camera.y;

	iPoint item_position;
	iPoint item_pivot;
	int rect_width = 0;
	int rect_height = 0;

	std::list<Unit*>::const_iterator item = actual_population.begin();
	while (item != actual_population.end())
	{
		rect_width = item._Ptr->_Myval->GetAnimation()->GetCurrentSprite()->GetFrame()->w;
		rect_height = item._Ptr->_Myval->GetAnimation()->GetCurrentSprite()->GetFrame()->h;
		
		fPoint pos = item._Ptr->_Myval->GetPosition();
		
		item_position = iPoint(pos.x, pos.y);
		item_pivot = { item._Ptr->_Myval->GetAnimation()->GetCurrentSprite()->GetXpivot(),item._Ptr->_Myval->GetAnimation()->GetCurrentSprite()->GetYpivot() };
		item_position -= item_pivot;

		if (x >= item_position.x && x <= item_position.x + rect_width && y >= item_position.y && y <= item_position.y + rect_height)
		{
			if (selected_elements.begin()._Ptr->_Myval != item._Ptr->_Myval)
			{
				selected_elements.clear();
				selected_elements.push_back(item._Ptr->_Myval);
				LOG("Entity Selected: Type: %s", selected_elements.begin()._Ptr->_Myval->GetName());
			}
			return;
		}
		item++;
	}

}

void j1Player::CheckPixelAlpha(int x, int y)
{

}

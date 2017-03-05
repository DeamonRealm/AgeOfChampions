#include "j1Player.h"

#include "p2Log.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Animator.h"

#include "j1App.h"
#include "j1Gui.h"
#include "j1EntitiesManager.h"

#include "UI_Element.h"
#include "UI_String.h"



Entity_Profile::Entity_Profile()
{
	element = nullptr;
	name = (UI_String*) App->gui->GenerateUI_Element(STRING);
	name->SetColor({ 255,255,255,255 });
	diplomacy = (UI_String*)App->gui->GenerateUI_Element(STRING);
	diplomacy->SetColor({ 255,255,255,255 });
}

Entity_Profile::~Entity_Profile()
{
}

void Entity_Profile::SetEntity(Entity * entity_selected)
{
	element = entity_selected;
	name->SetString((char*)element->GetName());
	 
	switch (element->GetDiplomacy())
	{
	case NEUTRAL: diplomacy->SetString("Neutral");
		break;
	case ALLY: diplomacy->SetString("Player");
		break;
	case ENEMY: diplomacy->SetString("Enemy");
		break;
	}
}

void Entity_Profile::DrawProfile() const
{
	App->render->Blit(App->gui->Get_UI_Texture(ICONS), 750, 500, &element->GetIcon());
	name->DrawAt(800, 500);
	diplomacy->DrawAt(800, 520);
	
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
	selection_rect = { 0,0,0,0 };
	Selected = new Entity_Profile();
	return true;
}

bool j1Player::PreUpdate()
{
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		App->input->GetMousePosition(selection_rect.x, selection_rect.y);
		Select_Entity();
	}

	if(App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
	{
		int x, y;
		App->input->GetMousePosition(x, y);
		Unit* new_unit = App->entities_manager->GenerateUnit(MILITIA);
		new_unit->SetPosition(x - App->render->camera.x, y - App->render->camera.y);
		new_unit->SetDiplomacy(ALLY);
		actual_population.push_back(new_unit);
	}
	if (App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN)
	{
		int x, y;
		App->input->GetMousePosition(x, y);
		Unit* new_unit = App->entities_manager->GenerateUnit(ARBALEST);
		new_unit->SetPosition(x - App->render->camera.x, y - App->render->camera.y);
		new_unit->SetDiplomacy(ALLY);
		actual_population.push_back(new_unit);
	}
	return true;
}

bool j1Player::PostUpdate()
{

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		Expand_SelectionRect();
	}
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
	{
		Select_Group();
		selection_rect = { 0,0,0,0 };
	}
	if (selected_elements.size() == 1) Selected->DrawProfile();

	return true;
}

bool j1Player::CleanUp()
{
	actual_population.clear();
	delete Selected;
	return true;
}

void j1Player::DrawGroup()
{
}

//Check if unit is inside selection rect
bool j1Player::UnitisIn(int x, int y, int width, int height)
{
	if (selection_rect.w < 0)
	{
		selection_rect.x += selection_rect.w;
		selection_rect.w = -selection_rect.w;
	}
	if (selection_rect.h < 0)
	{
		selection_rect.y += selection_rect.h;
		selection_rect.h = -selection_rect.h;
	}
	if (PointisIn(x,y)) return true;
	if (PointisIn(x + width, y + height)) return true;
	if (PointisIn(x + width,y)) return true;
	if (PointisIn(x, y + height)) return true;

	return false;
}

//Check if point is inside selection rect
bool j1Player::PointisIn(int x, int y)
{
	if (selection_rect.x < x && selection_rect.x + selection_rect.w > x && selection_rect.y < y && selection_rect.y + selection_rect.h > y) return true;
	else return false;
}

//Select one unit
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
				Selected->SetEntity(item._Ptr->_Myval);
			}
			return;
		}
		item++;
	}

}

//Select group of units (only Allys)
void j1Player::Select_Group()
{
	
	if (selection_rect.w == 0 || selection_rect.h == 0) return;
	selected_elements.clear();

	int x = 0, y = 0;
	int width = 0, height = 0;

	std::list<Unit*>::const_iterator item = actual_population.begin();
	while (item != actual_population.end())
	{
		if (item._Ptr->_Myval->GetDiplomacy() == ALLY)
		{
			x = item._Ptr->_Myval->GetPosition().x;
			x -= item._Ptr->_Myval->GetAnimation()->GetCurrentSprite()->GetXpivot();
			y = item._Ptr->_Myval->GetPosition().y;
			y -= item._Ptr->_Myval->GetAnimation()->GetCurrentSprite()->GetYpivot();
			width = item._Ptr->_Myval->GetSelectionRect()->w;
			height = item._Ptr->_Myval->GetSelectionRect()->h;
			
			if (UnitisIn(x, y, width, height))
			{
				selected_elements.push_back(item._Ptr->_Myval);
			}
		}
		item++;
	}
	LOG("Selected: %i", selected_elements.size());
}

//Expand Selection Rect
void j1Player::Expand_SelectionRect()
{
	App->input->GetMousePosition(selection_rect.w, selection_rect.h);

	selection_rect.w -= selection_rect.x;
	selection_rect.h -= selection_rect.y;
	
	App->render->DrawQuad({ selection_rect.x - App->render->camera.x, selection_rect.y - App->render->camera.y, selection_rect.w, selection_rect.h }, 255, 255, 255, 255, false);
}


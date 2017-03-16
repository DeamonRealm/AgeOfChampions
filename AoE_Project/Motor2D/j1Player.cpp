#include "j1Player.h"

#include "p2Log.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Animator.h"

#include "j1App.h"
#include "j1Window.h"
#include "j1Gui.h"
#include "j1EntitiesManager.h"

#include "UI_Element.h"
#include "UI_Image.h"
#include "UI_String.h"


//Entity Profile Constructor =====================================================
Entity_Profile::Entity_Profile()
{
	element = nullptr;
	name = (UI_String*) App->gui->GenerateUI_Element(STRING);
	name->SetColor({ 0,0,0,255 });
	diplomacy = (UI_String*)App->gui->GenerateUI_Element(STRING);
	diplomacy->SetColor({ 0,0,0,255 });
}

//Entity Profile Destructor ======================================================
Entity_Profile::~Entity_Profile()
{
}

//Functionality ==================================================================
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

Entity * Entity_Profile::GetEntity()
{
	return element;
}

void Entity_Profile::DrawProfile() const
{
	//Draw profile background
	App->render->DrawQuad({ 338 - App->render->camera.x, 628 - App->render->camera.y, 39, 41 }, 148, 148, 148);
	App->render->DrawQuad({ 340 - App->render->camera.x, 666 - App->render->camera.y, 36, 3 }, 255, 0, 0);

	//Draw life

	//Draw profile
	App->render->Blit(App->gui->Get_UI_Texture(ICONS), 340 - App->render->camera.x, 630 - App->render->camera.y, &element->GetIcon());
	name->DrawAt(390, 630);
	diplomacy->DrawAt(390, 650);
	
}

//j1Player Constructor ============================================================
j1Player::j1Player()
{
	name = "player";
}

//j1Player Destructor ============================================================
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
	game_hud = (UI_Image*) App->gui->GenerateUI_Element(IMG);
	game_hud->ChangeTextureId(HUD);
	game_hud->SetLayer(10);
	App->gui->PushScreen(game_hud);

	selection_rect = { 0,0,0,0 };
	Selected = new Entity_Profile();
	
	UI_Image*	profile;
	group_profile.reserve(max_selected_units);
	int i = 0;
	
	while (i < max_selected_units)
	{
		profile = (UI_Image*)App->gui->GenerateUI_Element(IMG);
		profile->Desactivate();
		profile->ChangeTextureId(ICONS);
		profile->SetBoxPosition(320, 630);
		group_profile.push_back(profile);
		i++;
	}

	// Entities Build ------------------------------------- only for testing recolection
	tree = App->entities_manager->GenerateResource(RESOURCE_TYPE::TREE);
	tree->SetPosition(130, 650);
	game_entityes.push_back(tree);
	berry_bush = App->entities_manager->GenerateResource(RESOURCE_TYPE::BERRY_BUSH);
	berry_bush->SetPosition(440, 380);
	game_entityes.push_back(berry_bush);
	gold_ore = App->entities_manager->GenerateResource(RESOURCE_TYPE::GOLD_ORE);
	gold_ore->SetPosition(300, 480);
	game_entityes.push_back(gold_ore);
	stone_ore = App->entities_manager->GenerateResource(RESOURCE_TYPE::STONE_ORE);
	stone_ore->SetPosition(200, 480);
	game_entityes.push_back(stone_ore);

	double_clickon = false;

	return true;
}

bool j1Player::PreUpdate()
{
	int x, y;
	App->input->GetMousePosition(x, y);

	UpperEntity = GetUpperEntity(x,y);
	if (UpperEntity != nullptr) App->gui->ChangeMouseTexture(SELECT);
	else App->gui->ChangeMouseTexture(DEFAULT);

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		selection_rect.x = x;
		selection_rect.y = y;
		Uint32 double_click_read = double_click.Read();

		if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT)
		{
			Select(ADD);
		}
		else Select(SINGLE);

		if ((selected_elements.size() == 1 && double_click_read == 0)|| (selected_elements.size() == 1 && double_click_read >= 250))
		{
			double_click.Start();
		}
		else if (double_click_read > 0 && double_click_read < 250)
		{
			double_clickon = true;
			Select(DOUBLECLICK);
		}
	}
	else if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		if (selected_elements.size() == 1 && selected_elements.begin()._Ptr->_Myval->GetEntityType() == UNIT)
		{
			LOG("World x%i y%i", x - App->render->camera.x, y - App->render->camera.y);

			App->entities_manager->SetUnitPath((Unit*)selected_elements.begin()._Ptr->_Myval, iPoint(x - App->render->camera.x, y - App->render->camera.y));
		}
		
		//test
		if (selected_elements.size() == 1) DoAction(RCLICK);
	}


	//Generate a town center in the mouse coordinates
	if(App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		Building* center = App->entities_manager->GenerateBuilding(BUILDING_TYPE::TOWN_CENTER);
		center->SetPosition(x - App->render->camera.x, y - App->render->camera.y);
		center->SetDiplomacy(ALLY);
		//game_entityes.push_back(center);
	}
	//Generate Villager in the mouse coordinates
	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	{
		Unit* new_unit = App->entities_manager->GenerateUnit(VILLAGER);
		new_unit->SetPosition(x - App->render->camera.x, y - App->render->camera.y);
		new_unit->SetDiplomacy(ALLY);
		actual_population.push_back(new_unit);
	}

	//Generate a Militia unit in the mouse coordinates
	if(App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
	{
		Unit* new_unit = App->entities_manager->GenerateUnit(MILITIA);
		new_unit->SetPosition(x - App->render->camera.x, y - App->render->camera.y);
		new_unit->SetDiplomacy(ALLY);
		actual_population.push_back(new_unit);
	}
	//Generate a Arbalest unit in the mouse coordinates
	if (App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN)
	{
		Unit* new_unit = App->entities_manager->GenerateUnit(ARBALEST);
		new_unit->SetPosition(x - App->render->camera.x, y - App->render->camera.y);
		new_unit->SetDiplomacy(ALLY);
		actual_population.push_back(new_unit);
	}

	return true;
}

bool j1Player::PostUpdate()
{
	game_hud->Draw(false);

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT && double_clickon == false)
	{
		Expand_SelectionRect();
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
	{
		if (double_clickon == true)
		{
			double_clickon = false;
		}
		else
		{
			Select(GROUP);
		}
		selection_rect = { 0,0,0,0 };
	}

	if (selected_elements.size() == 1) Selected->DrawProfile();
	else if (selected_elements.size() > 1) DrawGroup();



	//Draw Mouse Last one
	if (SDL_ShowCursor(-1) == 0) App->gui->DrawMouseTexture();

	return true;
}

bool j1Player::CleanUp()
{
	//Clear list and vectors
	actual_population.clear();
	selected_elements.clear();
	group_profile.clear();

	delete Selected;

	return true;
}

//Draw group_profiles
void j1Player::DrawGroup()
{
	SDL_Rect* box = nullptr;
	uint	  life = 0, max_life = 0;

	int i = selected_elements.size()-1;
	std::list<Entity*>::const_iterator item = selected_elements.end();
	while (item != selected_elements.begin())
	{
		item--;
		box = group_profile[i]->GetBox();
		//Draw profile background
		App->render->DrawQuad({ box->x - 2 - App->render->camera.x, box->y - 2 - App->render->camera.y, 39, 41 }, 138, 138, 138);
		App->render->DrawQuad({ box->x - App->render->camera.x, box->y +36 - App->render->camera.y, 36, 3 }, 255, 0, 0);

		//Draw life

		//Draw icon
		group_profile[i]->Draw(App->debug_mode);

		i--;
	}
}

bool j1Player::MoveSelectedUnits(int x, int y)
{
	return true;
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
	int camera_x, camera_y;
	camera_x = App->render->camera.x;
	camera_y = App->render->camera.y;

	if (PointisIn(x,y)) return true;
	if (PointisIn(x + width, y + height)) return true;
	if (PointisIn(x + width,y)) return true;
	if (PointisIn(x, y + height)) return true;

	return false;
}

//Check if point is inside selection rect
bool j1Player::PointisIn(int x, int y) const
{
	int camera_x, camera_y;
	camera_x = App->render->camera.x;
	camera_y = App->render->camera.y;

	if (selection_rect.x - camera_x < x && selection_rect.x - camera_x + selection_rect.w > x && selection_rect.y - camera_y < y && selection_rect.y - camera_y + selection_rect.h > y) return true;
	else return false;
}

//Select Entityes Function
void j1Player::Select(SELECT_TYPE type)
{
	if (type == GROUP || type == DOUBLECLICK)
	{
		UNIT_TYPE unit_type = NO_UNIT;
		int x = 0, y = 0;
		uint width = 0, height = 0;

		if (type == DOUBLECLICK)
		{
			if (Selected->GetEntity() == nullptr) return;
			else if (Selected->GetEntity()->GetEntityType() != UNIT) return;
			App->win->GetWindowSize(width, height);

			selection_rect = { 0, 32, (int)width, 560 };
			unit_type = ((Unit*)Selected->GetEntity())->GetUnitType();
		}
		else if (selection_rect.w == 0 || selection_rect.h == 0) return;

		UnSelect_Entity();

		//Select Entity
		std::list<Unit*>::const_iterator item = actual_population.begin();
		while (item != actual_population.end())
		{
			if (item._Ptr->_Myval->GetDiplomacy() != ALLY) ;
			else if (item._Ptr->_Myval->GetEntityType() != UNIT) ;
			else if (type == DOUBLECLICK && unit_type != item._Ptr->_Myval->GetUnitType()) ;
			else if (item._Ptr->_Myval->GetDiplomacy() == ALLY && selected_elements.size() < 60)
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
					item._Ptr->_Myval->Select();
				}
			}
			item++;
		}
	}
	else
	{
		if (type == SINGLE) UnSelect_Entity();

		if (UpperEntity == nullptr) return;
		UpperEntity->Select();

		if (type == SINGLE)
		{
			UnSelect_Entity();
			selected_elements.push_back(UpperEntity);
		}
		else if (UpperEntity->GetEntityType() == UNIT)
		{
			if (std::find(selected_elements.begin(), selected_elements.end(), UpperEntity) == selected_elements.end())
			{
				selected_elements.push_back(UpperEntity);
			}
		}
	}
	
	//Configure Selection Panel
	if (selected_elements.size() == 1) Selected->SetEntity(selected_elements.begin()._Ptr->_Myval);
	else if (selected_elements.size() > 1)
	{
		max_row_units = 16;
		SetGroupProfile();
	}
}

//Clean Selected_elements
void j1Player::UnSelect_Entity()
{
	std::list<Entity*>::const_iterator item = selected_elements.begin();
	while (item != selected_elements.end())
	{
		item._Ptr->_Myval->Deselect();
		item++;
	}
	selected_elements.clear();
}

//Expand Selection Rect
void j1Player::Expand_SelectionRect()
{
	App->input->GetMousePosition(selection_rect.w, selection_rect.h);

	selection_rect.w -= selection_rect.x;
	selection_rect.h -= selection_rect.y;
	
	App->render->DrawQuad({ selection_rect.x - App->render->camera.x, selection_rect.y - App->render->camera.y, selection_rect.w, selection_rect.h }, 255, 255, 255, 255, false);
}

//Return Upper Entity
Entity * j1Player::GetUpperEntity(int x, int y)const
{
//	int x = 0, y = 0;
	int width = 0, height = 0;

//	App->input->GetMousePosition(x, y);
	x -= App->render->camera.x;
	y -= App->render->camera.y;

	fPoint pos;
	iPoint item_position;
	iPoint item_pivot;

	Entity* ret = nullptr;
	std::list<Unit*>::const_iterator item = actual_population.begin();
	while (item != actual_population.end())
	{
		width = item._Ptr->_Myval->GetAnimation()->GetCurrentSprite()->GetFrame()->w;
		height = item._Ptr->_Myval->GetAnimation()->GetCurrentSprite()->GetFrame()->h;
		pos = item._Ptr->_Myval->GetPosition();
		item_position = iPoint(pos.x, pos.y);
		item_pivot = { item._Ptr->_Myval->GetAnimation()->GetCurrentSprite()->GetXpivot(),item._Ptr->_Myval->GetAnimation()->GetCurrentSprite()->GetYpivot() };
		item_position -= item_pivot;

		if (x >= item_position.x && x <= item_position.x + width && y >= item_position.y && y <= item_position.y + height)
		{
			if (ret == nullptr) ret = item._Ptr->_Myval;
			else if (ret->GetPosition().y <= item._Ptr->_Myval->GetPosition().y)
			{
				ret = item._Ptr->_Myval;
			}
		}
		item++;
	}
	if(ret == nullptr)
	{
		std::list<Entity*>::const_iterator item = game_entityes.begin();
		while (item != game_entityes.end())
		{
			width = item._Ptr->_Myval->GetAnimation()->GetCurrentSprite()->GetFrame()->w;
			height = item._Ptr->_Myval->GetAnimation()->GetCurrentSprite()->GetFrame()->h;
			pos = item._Ptr->_Myval->GetPosition();
			item_position = iPoint(pos.x, pos.y);
			item_pivot = { item._Ptr->_Myval->GetAnimation()->GetCurrentSprite()->GetXpivot(),item._Ptr->_Myval->GetAnimation()->GetCurrentSprite()->GetYpivot() };
			item_position -= item_pivot;

			if (x >= item_position.x && x <= item_position.x + width && y >= item_position.y && y <= item_position.y + height)
			{
				if (ret == nullptr) ret = item._Ptr->_Myval;
				else if (ret->GetPosition().y <= item._Ptr->_Myval->GetPosition().y)
				{
					ret = item._Ptr->_Myval;
				}
			}
			item++;
		}
	}
	return ret;
}

void j1Player::SetGroupProfile()
{
	int i = 0;
	int j = 0;

	std::list<Entity*>::const_iterator item = selected_elements.begin();
	while (item != selected_elements.end())
	{
		if (i%max_row_units == 0 && i != 0) j++;
		if (j == 3) {
			max_row_units++;
			SetGroupProfile();
			return;
		} 
		group_profile[i]->ChangeTextureRect(item._Ptr->_Myval->GetIcon());
		group_profile[i]->SetBoxPosition(340 + (i%max_row_units) * (row_size/max_row_units), 630 + j * 42);
		item++;
		i++;
	}
}

bool j1Player::DoAction(ACTION type)
{
	switch (type)
	{
	case LCLICK: 
		break;
	case RCLICK: 
		if (Selected->GetEntity()->GetEntityType() == VILLAGER && UpperEntity->GetEntityType() == RESOURCE)
		{
		((Unit*)Selected->GetEntity())->SetInteractionTarget(UpperEntity);
		return true;
		}
		break;
	default: return false;
		break;
	}
	return false;
}

#include "Hud_SelectionPanel.h"

//Modules
#include "j1App.h"
#include "j1Render.h"
#include "j1Gui.h"
#include "j1Input.h"
#include "j1Window.h"
#include "j1EntitiesManager.h"
#include "j1Animator.h"

#include "SDL\include\SDL_rect.h"

//GUI Elements
#include "UI_String.h"
#include "UI_Image.h"


//Entityes Elements
#include "BaseEntities.h"

//Entity Profile Constructor =====================================================
Entity_Profile::Entity_Profile() : element(nullptr), e_type(NO_ENTITY), isenemy(false), life_update(0), u_attack(0), u_deffence(0), u_range(0),
u_capacity(0), u_resources(0), m_capacity(0), m_life(0)
{
	name = (UI_String*)App->gui->GenerateUI_Element(STRING);
	name->SetColor({ 0,0,0,255 });

	diplomacy = (UI_String*)App->gui->GenerateUI_Element(STRING);
	diplomacy->SetColor({ 255,0,0,0 });
	diplomacy->SetString("Enemy");

	//life
	life = (UI_String*)App->gui->GenerateUI_Element(STRING);
	life->SetColor({ 0,0,0,255 });

	//Units and Buildings
	attack = (UI_String*)App->gui->GenerateUI_Element(STRING);
	attack->SetColor({ 0,0,0,255 });

	deffence = (UI_String*)App->gui->GenerateUI_Element(STRING);
	deffence->SetColor({ 0,0,0,255 });

	range = (UI_String*)App->gui->GenerateUI_Element(STRING);
	range->SetColor({ 0,0,0,255 });

	//Buildings
	capacity = (UI_String*)App->gui->GenerateUI_Element(STRING);
	capacity->SetColor({ 0,0,0,255 });

	//resources
	resources = (UI_String*)App->gui->GenerateUI_Element(STRING);
	resources->SetColor({ 0,0,0,255 });

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

	life_update = element->GetLife();
	if (life_update > 0)
	{
		m_life = element->GetMaxLife();
		profile_text.clear();
		profile_text = App->gui->SetStringFromInt(life_update);
		profile_text = profile_text + "/" + App->gui->SetStringFromInt(m_life);
		life->SetString((char*)profile_text.c_str());
	}

	if (element->GetDiplomacy() == ENEMY)
	{
		isenemy = true;
	}
	else isenemy = false;

	e_type = element->GetEntityType();
	if (e_type == UNIT)
	{
		u_attack = ((Unit*)element)->GetAttackHitPoints();
		if (u_attack > 0)attack->SetString(App->gui->SetStringFromInt(u_attack));

		u_deffence = ((Unit*)element)->GetDefense();
		if (u_deffence > 0)deffence->SetString(App->gui->SetStringFromInt(u_deffence));

		u_range = ((Unit*)element)->GetAttackRange();
		if (u_range > 0) range->SetString(App->gui->SetStringFromInt(u_range));
	}
	else if (e_type == BUILDING)
	{
		u_capacity = ((Building*)element)->GetCurrentUnits();
		m_capacity = ((Building*)element)->GetUnitsCapacity();
		profile_text.clear();
		profile_text = App->gui->SetStringFromInt(u_capacity);
		profile_text = profile_text + "/" + App->gui->SetStringFromInt(m_capacity);
		capacity->SetString((char*)profile_text.c_str());
	}
	else if (e_type = RESOURCE)
	{
		u_resources = ((Resource*)element)->GetMaxResources();
		resources->SetString(App->gui->SetStringFromInt(u_resources));
	}
}

Entity * Entity_Profile::GetEntity()
{
	return element;
}

void Entity_Profile::DrawProfile() const
{
	//Draw profile background
	App->render->DrawQuad({ 338 - App->render->camera.x, 632 - App->render->camera.y, 39, 38 }, 148, 148, 148);

	//Draw profile icon
	App->render->Blit(App->gui->Get_UI_Texture(ICONS), 340 - App->render->camera.x, 634 - App->render->camera.y, &element->GetIcon());

	name->DrawAt(337, 610);
	//civilization->DrawAt(472, 640)
	if (isenemy) diplomacy->DrawAt(472, 660);

	//Draw life
	if (life_update != 0)
	{
		App->render->DrawQuad({ 340 - App->render->camera.x, 670 - App->render->camera.y, 36, 3 }, 255, 0, 0);
		App->render->DrawQuad({ 340 - App->render->camera.x, 670 - App->render->camera.y, 36 * (m_life / life_update), 3 }, 0, 255, 0);

		life->DrawAt(380, 660);
	}

	SDL_Rect rect = { 0,0,0,0 };
	if (e_type == UNIT)
	{
		if (u_attack > 0)
		{
			rect = { 372,287,36,23 };
			App->render->Blit(App->gui->Get_UI_Texture(ICONS), 340 - App->render->camera.x, 680 - App->render->camera.y, &rect);
			attack->DrawAt(390, 680);
		}
		if (u_deffence > 0)
		{
			rect = { 336,287,36,23 };
			App->render->Blit(App->gui->Get_UI_Texture(ICONS), 340 - App->render->camera.x, 700 - App->render->camera.y, &rect);
			deffence->DrawAt(390, 700);
		}
		if (u_range > 0)
		{
			rect = { 300,287,36,21 };
			App->render->Blit(App->gui->Get_UI_Texture(ICONS), 340 - App->render->camera.x, 720 - App->render->camera.y, &rect);
			range->DrawAt(390, 720);
		}
	}
	else if (e_type == BUILDING)
	{
		if (m_capacity != 0)
		{
			rect = { 624,287,36,21 };
			App->render->Blit(App->gui->Get_UI_Texture(ICONS), 340 - App->render->camera.x, 680 - App->render->camera.y, &rect);
			capacity->DrawAt(390, 680);
		}
		if (u_attack > 0)
		{
			rect = { 372,287,36,23 };
			App->render->Blit(App->gui->Get_UI_Texture(ICONS), 340 - App->render->camera.x, 700 - App->render->camera.y, &rect);
			attack->DrawAt(390, 700);
		}
		if (u_range > 0)
		{
			rect = { 300,287,36,21 };
			App->render->Blit(App->gui->Get_UI_Texture(ICONS), 340 - App->render->camera.x, 720 - App->render->camera.y, &rect);
			range->DrawAt(390, 720);
		}
	}
	else if (e_type == RESOURCE)
	{
		resources->DrawAt(390, 680);
	}
}

void Entity_Profile::UpdateStats()
{
	if (element->GetLife() != life_update)
	{
		life_update = element->GetLife();
		profile_text.clear();
		profile_text = App->gui->SetStringFromInt(life_update);
		profile_text = profile_text + "/" + App->gui->SetStringFromInt(element->GetMaxLife());
		life->SetString((char*)profile_text.c_str());
	}
	if (e_type == BUILDING)
	{
		if (u_capacity != ((Building*)element)->GetCurrentUnits())
		{
			u_capacity = ((Building*)element)->GetCurrentUnits();
			profile_text.clear();
			profile_text = App->gui->SetStringFromInt(u_capacity);
			profile_text = profile_text + "/" + App->gui->SetStringFromInt(m_capacity);
			capacity->SetString((char*)profile_text.c_str());
		}
	}
	if (e_type == RESOURCE)
	{
		if (u_resources != ((Resource*)element)->GetMaxResources())
		{
			u_resources = ((Resource*)element)->GetMaxResources();
			resources->SetString(App->gui->SetStringFromInt(u_resources));
		}
	}
}


Selection_Panel::Selection_Panel() : selection_rect({0,0,0,0}), map_viewport({ 0, 32, 1366, 560 }), double_clickon(false), expand(false)
{
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
}

Selection_Panel::~Selection_Panel()
{
	CleanUp();
}


bool Selection_Panel::PreUpdate()
{	
	App->input->GetMousePosition(mouse_x, mouse_y);

	if (PointisInViewport(mouse_x, mouse_y) == false) return false;
	// Calculate upper entity
	UpperEntity = GetUpperEntity(mouse_x, mouse_y);
	if (UpperEntity != nullptr) App->gui->ChangeMouseTexture(SELECT);
	else App->gui->ChangeMouseTexture(DEFAULT); 

	return true;
}


bool Selection_Panel::CleanUp()
{
	selected_elements.clear();
	group_profile.clear();

	delete Selected;

	return true;
}

void Selection_Panel::Handle_Input(GUI_INPUT newevent)
{
	switch (newevent)
	{
	case UP_ARROW:
		break;
	case DOWN_ARROW:
		break;
	case LEFT_ARROW:
		break;
	case RIGHT_ARROW:
		break;
	case MOUSE_LEFT_BUTTON_DOWN:
	{
		selection_rect.x = mouse_x;
		selection_rect.y = mouse_y;
		Uint32 double_click_read = double_click.Read();

		if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT)
		{
			Select(ADD);
		}
		else Select(SINGLE);

		expand = true;

		if ((selected_elements.size() == 1 && double_click_read == 0) || (selected_elements.size() == 1 && double_click_read >= 250))
		{
			double_click.Start();
		}
		else if (double_click_read > 0 && double_click_read < 250)
		{
			double_clickon = true;
			expand = false;
			Select(DOUBLECLICK);
		}

	}
		break;
	case MOUSE_LEFT_BUTTON_REPEAT:
		if (expand == true)
		{
			Expand_SelectionRect();
		}
		break;
	case MOUSE_LEFT_BUTTON_UP:
	{
		if (double_clickon == true)
		{
			double_clickon = false;
		}
		else
		{
			Select(GROUP);
			expand = false;
		}
		selection_rect = { 0,0,0,0 };
	}
		break;
	case MOUSE_RIGHT_BUTTON:
		if (selected_elements.size() == 1 && selected_elements.begin()._Ptr->_Myval->GetEntityType() == UNIT)
		{
			App->entities_manager->SetUnitPath((Unit*)selected_elements.begin()._Ptr->_Myval, iPoint(mouse_x - App->render->camera.x, mouse_y - App->render->camera.y));
		}
		break;
	case MOUSE_IN:
		break;
	case MOUSE_OUT:
		break;
	case SUPR:
		break;
	case BACKSPACE:
		break;
	case ENTER:
		break;
	case TAB:
		break;
	default:
		break;
	}
}

bool Selection_Panel::Draw()
{
	if (selected_elements.size() == 1)
	{
		Selected->UpdateStats();
		Selected->DrawProfile();
	}
	else if (selected_elements.size() > 1) DrawGroup();

	return true;
}

void Selection_Panel::DrawGroup()
{
	SDL_Rect* box = nullptr;
	int	  life = 0, max_life = 0;

	int i = selected_elements.size() - 1;
	std::list<Entity*>::const_iterator item = selected_elements.end();
	while (item != selected_elements.begin())
	{
		item--;

		life = item._Ptr->_Myval->GetLife();
		max_life = item._Ptr->_Myval->GetMaxLife();

		box = group_profile[i]->GetBox();
		//Draw profile background
		App->render->DrawQuad({ box->x - 2 - App->render->camera.x, box->y - 2 - App->render->camera.y, 39, 41 }, 138, 138, 138);

		//Draw life
		App->render->DrawQuad({ box->x - App->render->camera.x, box->y + 36 - App->render->camera.y, 36, 3 }, 255, 0, 0);
		App->render->DrawQuad({ box->x - App->render->camera.x, box->y + 36 - App->render->camera.y, 36 * (max_life / life), 3 }, 0, 255, 0);

		//Draw icon
		group_profile[i]->Draw(App->debug_mode);

		i--;
	}
}

//Selection Functions
bool Selection_Panel::UnitisIn(int x, int y, int width, int height)
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

	if (PointisIn(x, y)) return true;
	if (PointisIn(x + width, y + height)) return true;
	if (PointisIn(x + width, y)) return true;
	if (PointisIn(x, y + height)) return true;

	return false;
}

bool Selection_Panel::PointisIn(int x, int y) const
{
	int camera_x, camera_y;
	camera_x = App->render->camera.x;
	camera_y = App->render->camera.y;

	if (selection_rect.x - camera_x < x && selection_rect.x - camera_x + selection_rect.w > x && selection_rect.y - camera_y < y && selection_rect.y - camera_y + selection_rect.h > y) return true;
	else return false;
	return false;
}

bool Selection_Panel::PointisInViewport(int x, int y) 
{
	if (x == 0 && y == 0) App->input->GetMousePosition(mouse_x, mouse_y);
	// Continue PreUpdate if mouse is inside map
	if (map_viewport.x< mouse_x && map_viewport.x + map_viewport.w > mouse_x
		&& map_viewport.y< mouse_y && map_viewport.y + map_viewport.h > mouse_y) return true;
	else return false;

}

// Change to quads
void Selection_Panel::Select(SELECT_TYPE type)
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
		std::list<Unit*>::const_iterator item = App->entities_manager->UnitsList()->begin();
		while (item != App->entities_manager->UnitsList()->end())
		{
			if (item._Ptr->_Myval->GetDiplomacy() != ALLY);
			else if (item._Ptr->_Myval->GetEntityType() != UNIT);
			else if (type == DOUBLECLICK && unit_type != item._Ptr->_Myval->GetUnitType());
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

void Selection_Panel::UnSelect_Entity()
{
	std::list<Entity*>::const_iterator item = selected_elements.begin();
	while (item != selected_elements.end())
	{
		item._Ptr->_Myval->Deselect();
		item++;
	}
	selected_elements.clear();
}

void Selection_Panel::Expand_SelectionRect()
{
	selection_rect.w = mouse_x;
	if (mouse_y > map_viewport.y + map_viewport.h) selection_rect.h = map_viewport.y + map_viewport.h;
	else if (mouse_y < map_viewport.y) selection_rect.h = map_viewport.y;
	else selection_rect.h = mouse_y;

	selection_rect.w -= selection_rect.x;
	selection_rect.h -= selection_rect.y;

	App->render->DrawQuad({ selection_rect.x - App->render->camera.x, selection_rect.y - App->render->camera.y, selection_rect.w, selection_rect.h }, 255, 255, 255, 255, false);
}

// Change to quads
Entity * Selection_Panel::GetUpperEntity(int x, int y) const
{
	int width = 0, height = 0;

	x -= App->render->camera.x;
	y -= App->render->camera.y;

	fPoint pos;
	iPoint item_position;
	iPoint item_pivot;

	Entity* ret = nullptr;
	std::list<Unit*>::const_iterator unit = App->entities_manager->UnitsList()->begin();
	while (unit != App->entities_manager->UnitsList()->end())
	{
		width = unit._Ptr->_Myval->GetAnimation()->GetCurrentSprite()->GetFrame()->w;
		height = unit._Ptr->_Myval->GetAnimation()->GetCurrentSprite()->GetFrame()->h;
		pos = unit._Ptr->_Myval->GetPosition();
		item_position = iPoint(pos.x, pos.y);
		item_pivot = { unit._Ptr->_Myval->GetAnimation()->GetCurrentSprite()->GetXpivot(),unit._Ptr->_Myval->GetAnimation()->GetCurrentSprite()->GetYpivot() };
		item_position -= item_pivot;

		if (x >= item_position.x && x <= item_position.x + width && y >= item_position.y && y <= item_position.y + height)
		{
			if (ret == nullptr) ret = unit._Ptr->_Myval;
			else if (ret->GetPosition().y <= unit._Ptr->_Myval->GetPosition().y)
			{
				ret = unit._Ptr->_Myval;
			}
		}
		unit++;
	}
	
	std::list<Building*>::const_iterator building = App->entities_manager->BuildingList()->begin();
	while (building != App->entities_manager->BuildingList()->end())
	{
		width = building._Ptr->_Myval->GetSelectionRect()->w;
		height = building._Ptr->_Myval->GetSelectionRect()->h;
		pos = building._Ptr->_Myval->GetPosition();
		item_position = iPoint(pos.x, pos.y);
		item_pivot = {width/2,height/2 };
		item_position -= item_pivot;

		if (x >= item_position.x && x <= item_position.x + width && y >= item_position.y && y <= item_position.y + height)
		{
			if (ret == nullptr) ret = building._Ptr->_Myval;
			else if (ret->GetPosition().y <= building._Ptr->_Myval->GetPosition().y)
			{
				ret = building._Ptr->_Myval;
			}
		}
		building++;
	}

	std::list<Resource*>::const_iterator item = App->entities_manager->ResourceList()->begin();
	while (item != App->entities_manager->ResourceList()->end())
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
	return ret;
}

void Selection_Panel::SetGroupProfile()
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
		group_profile[i]->SetBoxPosition(340 + (i%max_row_units) * (row_size / max_row_units), 630 + j * 42);
		item++;
		i++;
	}
}

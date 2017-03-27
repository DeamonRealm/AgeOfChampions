#include "Hud_SelectionPanel.h"
#include "Hud_ActionPanel.h"

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


//Entities Elements
#include "BaseEntities.h"


//Actions headers
#include "Actions_Unit.h"
#include "Actions_Building.h"

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
	if (entity_selected == nullptr) return;
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
		u_resources = ((Resource*)element)->GetMaxLife();
		resources->SetString(App->gui->SetStringFromInt(u_resources));
	}
}

Entity * Entity_Profile::GetEntity()
{
	return element;
}

void Entity_Profile::DrawProfile() const
{
	if (element == nullptr) return;
	//Draw profile background
	App->render->DrawQuad({ 338 - App->render->camera.x, 632 - App->render->camera.y, 39, 38 }, 148, 148, 148);

	//Draw profile icon
	App->render->Blit(App->gui->Get_UI_Texture(ICONS), 340 - App->render->camera.x, 634 - App->render->camera.y, &element->GetIcon());

	name->DrawAt(337, 610);
	//civilization->DrawAt(472, 640)
	if (isenemy) diplomacy->DrawAt(472, 660);

	//Draw life
	if (life_update >= 0 && e_type == UNIT)
	{
		App->render->DrawQuad({ 340 - App->render->camera.x, 670 - App->render->camera.y, 36, 3 }, 255, 0, 0);
		App->render->DrawQuad({ 340 - App->render->camera.x, 670 - App->render->camera.y, 36 * life_update / m_life, 3 }, 0, 255, 0);

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
		if (life_update == 0)
		{
			element->Deselect();
			element = nullptr;
			return;
		}
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
		if (u_resources != ((Resource*)element)->GetMaxLife())
		{
			u_resources = ((Resource*)element)->GetLife();
			resources->SetString(App->gui->SetStringFromInt(u_resources));
		}
	}
}


Selection_Panel::Selection_Panel() : selection_rect({0,0,0,0}), map_viewport({ 0, 32, 1366, 560 }), double_clickon(false), expand(false)
{
	Selected = new Entity_Profile();

	UI_Image*	profile;
	group_profile.reserve(max_selected_units);
	unit_quad_selection.reserve(max_selected_units);
	building_quad_selection.reserve(max_selected_units);
	resource_quad_selection.reserve(max_selected_units);

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

	

	if (selected_elements.size() == 0 || PointisInViewport(mouse_x, mouse_y) == false ) return false;
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
			if (Selected->GetEntity() == nullptr || UpperEntity == nullptr)break;

			//Set entity target to the selected unit
			/*Selected->GetEntity()->GetWorker()->Reset();
			Selected->GetEntity()->AddAction(App->action_manager->MoveAction((Unit*)Selected->GetEntity(), mouse_x - App->render->camera.x, mouse_y - App->render->camera.y));*/
			
			//Selected->GetEntity()->AddAction(App->action_manager->AttackToUnitAction((Unit*)Selected->GetEntity(), (Unit*)UpperEntity));
			Selected->GetEntity()->AddAction(App->action_manager->RecollectAction((Villager*)Selected->GetEntity(), (Resource*)UpperEntity));

		}


		if (selected_elements.size() == 1 && selected_elements.begin()._Ptr->_Myval->GetEntityType() == BUILDING)
		{
			//Set entity target to the selected unit
			//Selected->GetEntity()->GetWorker()->Reset();
			Selected->GetEntity()->AddAction(new SpawnUnitAction((Building*)Selected->GetEntity(), ARBALEST));
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
		if (Selected->GetEntity() == nullptr) selected_elements.clear();
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
		if (life <= 0)
		{
			item._Ptr->_Myval->Deselect();
			selected_elements.remove(item._Ptr->_Myval);
			continue;
		}
		max_life = item._Ptr->_Myval->GetMaxLife();

		box = group_profile[i]->GetBox();
		//Draw profile background
		App->render->DrawQuad({ box->x - 2 - App->render->camera.x, box->y - 2 - App->render->camera.y, 39, 41 }, 138, 138, 138);

		//Draw life
		App->render->DrawQuad({ box->x - App->render->camera.x, box->y + 36 - App->render->camera.y, 36, 3 }, 255, 0, 0);
		App->render->DrawQuad({ box->x - App->render->camera.x, box->y + 36 - App->render->camera.y, 36 * life/max_life, 3 }, 0, 255, 0);

		//Draw icon
		group_profile[i]->Draw(App->debug_mode);

		i--;
	}
}

bool Selection_Panel::PointisInViewport(int x, int y) 
{
	SDL_Point p = { x,y };
	return SDL_PointInRect(&p, &map_viewport);
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
			
			selection_rect = { 0, 32 , (int)width, 560 };
			unit_type = ((Unit*)Selected->GetEntity())->GetUnitType();

		}
		else if (selection_rect.w == 0 || selection_rect.h == 0) return;
		else
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
		}

		selection_rect.x -= App->render->camera.x;
		selection_rect.y -= App->render->camera.y;
		

		unit_quad_selection.clear();
		UnSelect_Entity();

		App->entities_manager->units_quadtree.CollectCandidates(unit_quad_selection, selection_rect);

		//Select Entity
		int size = unit_quad_selection.size();
		for(int count = 0; count < size; count++)
		{
			if (unit_quad_selection[count]->GetDiplomacy() != ALLY);
			else if (unit_quad_selection[count]->GetEntityType() != UNIT);
			else if (type == DOUBLECLICK && unit_type != unit_quad_selection[count]->GetUnitType());
			else 
			{
					selected_elements.push_back(unit_quad_selection[count]);
					unit_quad_selection[count]->Select();
			}
		}
	}
	else
	{
		if (type == SINGLE) UnSelect_Entity();

		UpperEntity = GetUpperEntity(mouse_x,mouse_y);
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
	if (selected_elements.size() > 1)
	{
		max_row_units = 16;
		SetGroupProfile();
	}
	if(selected_elements.size() >= 1) Selected->SetEntity(selected_elements.begin()._Ptr->_Myval);
	else Selected->SetEntity(nullptr);
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
Entity * Selection_Panel::GetUpperEntity(int x, int y) 
{
	Entity* ret = nullptr;

	int count = 0, size = 0;
	int width = 0, height = 0;
	Sprite* current_sprite = nullptr;
	SDL_Rect rect;
	fPoint pos;

	x -= App->render->camera.x;
	y -= App->render->camera.y;

	rect = map_viewport;
	rect.x -= App->render->camera.x;
	rect.y -= App->render->camera.y;

	unit_quad_selection.clear();
	building_quad_selection.clear();
	resource_quad_selection.clear();

	App->entities_manager->units_quadtree.CollectCandidates(unit_quad_selection, rect);
	App->entities_manager->buildings_quadtree.CollectCandidates(building_quad_selection, rect);
	App->entities_manager->resources_quadtree.CollectCandidates(resource_quad_selection, rect);

	// Search Units
	size = unit_quad_selection.size();
	for (count = 0; count < size; count++)
	{
		current_sprite = (Sprite*) unit_quad_selection[count]->GetAnimation()->GetCurrentSprite();
		rect = *current_sprite->GetFrame();
		pos = unit_quad_selection[count]->GetPosition();
		rect.x = (int)pos.x - current_sprite->GetXpivot();
		rect.y = (int)pos.y - current_sprite->GetYpivot();
	
		if (x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h)
		{
			if (ret == nullptr) ret =unit_quad_selection[count];
			else if (ret->GetPosition().y < unit_quad_selection[count]->GetPosition().y)
			{
				ret = unit_quad_selection[count];
			}
		}
	}
	
	// Search Buildings
	size = building_quad_selection.size();
	for (count = 0; count < size; count++)
	{
		rect = *building_quad_selection[count]->GetSelectionRect();
		pos = building_quad_selection[count]->GetPosition();
		rect.x = (int)pos.x - rect.w / 2;
		rect.y = (int)pos.y - rect.h / 2;

		if (x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h)
		{
			if (ret == nullptr) ret = building_quad_selection[count];
			else if (ret->GetPosition().y < building_quad_selection[count]->GetPosition().y)
			{
				ret = building_quad_selection[count];
			}
		}
	}
	// Search Resources;
	size = resource_quad_selection.size();
	for (count = 0; count < size; count++)
	{
		rect = *resource_quad_selection[count]->GetSelectionRect();
		pos = resource_quad_selection[count]->GetPosition();
		rect.x = (int)pos.x - rect.w / 2;
		rect.y = (int)pos.y - rect.h / 2;

		if (x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h)
		{
			if (ret == nullptr) ret = resource_quad_selection[count];
			else if (ret->GetPosition().y <= resource_quad_selection[count]->GetPosition().y)
			{
				ret = resource_quad_selection[count];
			}
		}
	}
	return ret;
}

void Selection_Panel::SetGroupProfile()
{
	int i = 0;
	int j = 0;
	int size = selected_elements.size();
	max_row_units = 16;
	for (; max_row_units < 20; max_row_units++)
	{
		if ((float)size / max_row_units <= 3) break;
	}

	std::list<Entity*>::const_iterator item = selected_elements.begin();
	while (item != selected_elements.end())
	{
		if (i%max_row_units == 0 && i != 0) j++;
		group_profile[i]->ChangeTextureRect(item._Ptr->_Myval->GetIcon());
		group_profile[i]->SetBoxPosition(340 + (i%max_row_units) * (row_size / max_row_units), 630 + j * 42);
		item++;
		i++;
	}
}

Entity * Selection_Panel::GetSelected() const
{
	if (selected_elements.size() > 0) return selected_elements.begin()._Ptr->_Myval;
	else return nullptr;
}

uint Selection_Panel::GetSelectedSize() const
{
	return selected_elements.size();
}

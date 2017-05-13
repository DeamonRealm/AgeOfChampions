#include "Hud_SelectionPanel.h"
#include "Hud_ActionPanel.h"
#include "Hud_MinimapPanel.h"

//Modules
#include "j1App.h"
#include "j1Render.h"
#include "j1Gui.h"
#include "j1Input.h"
#include "j1Window.h"
#include "j1EntitiesManager.h"
#include "j1Animator.h"
#include "j1GroupMovement.h"
#include "j1FogOfWar.h"
#include "SDL\include\SDL_rect.h"
#include "j1Map.h"
#include "j1BuffManager.h"
//GUI Elements
#include "UI_String.h"
#include "UI_Image.h"


//Entities Elements
#include "BaseEntities.h"


//Actions headers
#include "Actions_Unit.h"
#include "Actions_Building.h"

/*This is temp*/ #include "j1BuffManager.h"

//Entity Profile Constructor =====================================================
Entity_Profile::Entity_Profile() : element(nullptr), e_type(NO_ENTITY), isenemy(false), life_update(0), u_attack(0), u_deffence(0), u_range(0),
u_capacity(0), m_capacity(0), m_life(0)
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
}

//Entity Profile Destructor ======================================================
Entity_Profile::~Entity_Profile()
{
	element = nullptr;
}

//Functionality ==================================================================
void Entity_Profile::SetEntity(Entity * entity_selected)
{
	if (entity_selected == nullptr)
	{
		entity_selected = nullptr;
		return;
	}
	element = entity_selected;
	background = { 650,371,40,40 };

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

	u_range = 0;
	u_deffence = 0;
	deffence_up = 0;
	u_attack = 0;
	attack_up = 0;

	if (element->GetDiplomacy() == ENEMY)
	{
		isenemy = true;
	}
	else isenemy = false;

	e_type = element->GetEntityType();
	if (e_type == UNIT)
	{
		u_attack = ((Unit*)element)->GetAttackHitPoints();
		attack_up = (int)ceil(((Unit*)element)->GetAttackBuff());
		if (u_attack > 0 && attack_up == 0)attack->SetString(App->gui->SetStringFromInt(u_attack));
		else if (attack_up > 0)
		{
			profile_text.clear();
			profile_text = App->gui->SetStringFromInt(u_attack);
			profile_text = profile_text + " + " + App->gui->SetStringFromInt(attack_up);
			attack->SetString((char*)profile_text.c_str());
		}

		u_deffence = ((Unit*)element)->GetDefense();
		deffence_up = (int)ceil(((Unit*)element)->GetDefenseBuff());
		if (u_deffence > 0 && deffence_up == 0)deffence->SetString(App->gui->SetStringFromInt(u_deffence));
		else if (deffence_up > 0)
		{
			profile_text.clear();
			profile_text = App->gui->SetStringFromInt(u_deffence);
			profile_text = profile_text + " + " + App->gui->SetStringFromInt(deffence_up);
			attack->SetString((char*)profile_text.c_str());
		}
		u_range = ((Unit*)element)->GetAttackRange();
		if (u_range > 0) range->SetString(App->gui->SetStringFromInt(u_range));
	}
	else if (e_type == BUILDING)
	{
		u_capacity = ((HabitableBuilding*)element)->GetCurrentUnits();
		m_capacity = ((HabitableBuilding*)element)->GetUnitsCapacity();
		profile_text.clear();
		profile_text = App->gui->SetStringFromInt(u_capacity);
		profile_text = profile_text + "/" + App->gui->SetStringFromInt(m_capacity);
		capacity->SetString((char*)profile_text.c_str());
	
	}
}

Entity * Entity_Profile::GetEntity()
{
	return element;
}

void Entity_Profile::Reset()
{
	profile_text.clear();
	element = nullptr;
	life_update = 0;
	m_life = 0;
	u_attack = 0;
	u_deffence = 0;
	attack_up = 0;
	deffence_up = 0;
	u_range = 0;
	u_capacity = 0;
	m_capacity = 0;
}

void Entity_Profile::DrawProfile() const
{
	if (element == nullptr) return;
	App->render->Blit(App->gui->Get_UI_Texture(ICONS), 342 - App->render->camera.x, 634 - App->render->camera.y, &element->GetIcon());

	//Draw profile icon
	App->render->Blit(App->gui->Get_UI_Texture(ICONS), 340 - App->render->camera.x, 634 - App->render->camera.y, &background);

	name->DrawAt(337, 615);
	if (isenemy) diplomacy->DrawAt(472, 660);

	//Draw life
	if (life_update >= 0)
	{
		App->render->DrawQuad({ 342 - App->render->camera.x, 670 - App->render->camera.y, 36, 2 }, 255, 0, 0);
		App->render->DrawQuad({ 342 - App->render->camera.x, 670 - App->render->camera.y, 36 * life_update / m_life, 2 }, 0, 255, 0);

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
}

void Entity_Profile::UpdateStats()
{
	if (element == nullptr) return;

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
	if (e_type == UNIT)
	{
		int tmp = (int)ceil(((Unit*)element)->GetAttackBuff());
		((Unit*)element)->GetAttackHitPoints();
		if (attack_up != tmp)
		{
			attack_up = tmp;
			if (attack_up == 0)attack->SetString(App->gui->SetStringFromInt(u_attack));
			else if (attack_up > 0)
			{
				profile_text.clear();
				profile_text = App->gui->SetStringFromInt(u_attack);
				profile_text = profile_text + " + " + App->gui->SetStringFromInt(attack_up);
				attack->SetString((char*)profile_text.c_str());
			}
		}

		u_deffence = ((Unit*)element)->GetDefense();

		if (deffence_up != ceil(((Unit*)element)->GetDefenseBuff()))
		{
			deffence_up = (int)ceil(((Unit*)element)->GetDefenseBuff());
			if (deffence_up == 0)deffence->SetString(App->gui->SetStringFromInt(u_deffence));
			else if (deffence_up > 0)
			{
				profile_text.clear();
				profile_text = App->gui->SetStringFromInt(u_deffence);
				profile_text = profile_text + " + " + App->gui->SetStringFromInt(deffence_up);
				attack->SetString((char*)profile_text.c_str());
			}
		}
	}
	if (e_type == BUILDING)
	{
		if (u_capacity != ((HabitableBuilding*)element)->GetCurrentUnits())
		{
			u_capacity = ((HabitableBuilding*)element)->GetCurrentUnits();
			profile_text.clear();
			profile_text = App->gui->SetStringFromInt(u_capacity);
			profile_text = profile_text + "/" + App->gui->SetStringFromInt(m_capacity);
			capacity->SetString((char*)profile_text.c_str());
		}
	}
}


Selection_Panel::Selection_Panel() : selection_rect({ 0,0,0,0 }), map_viewport({ 0, 32, 1366, 560 }), double_clickon(false), expand(false), inviewport(false)
{
	App->gui->SetDefaultInputTarget((j1Module*)App->player);

	viewport = App->gui->GenerateUI_Element(UNDEFINED);
	viewport->SetBox(map_viewport);
	viewport->Activate();
	viewport->SetInputTarget((j1Module*)App->player);

	Selected = new Entity_Profile();

	UI_Image*	profile;
	group_profile.reserve(max_selected_units);
	unit_quad_selection.reserve(max_selected_units);
	building_quad_selection.reserve(max_selected_units);
	resource_quad_selection.reserve(max_selected_units);

	int i = 0;

	while (i < (int)max_selected_units)
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


	if (WindowsMove()) App->map->CalculateTilesInView();


	if (selected_elements.size() == 0 || inviewport == false || refresh_upperentity.Read() < REFRESH_RATE) return false;
	refresh_upperentity.Start();

	// Calculate upper entity
	UpperEntity = GetUpperEntity(mouse_x, mouse_y);
	if (UpperEntity != nullptr)
	{
		if (selected_diplomacy == ALLY && UpperEntity->GetDiplomacy() == ENEMY) App->gui->ChangeMouseTexture(ATTACK);
		else App->gui->ChangeMouseTexture(DEFAULT);
	}
	else App->gui->ChangeMouseTexture(DEFAULT);

	return true;
}


bool Selection_Panel::CleanUp()
{
	selected_elements.clear();
	group_profile.clear();

	if (Selected != nullptr)
	{
		delete Selected;
	}

	if (arrows_particle != nullptr)
	{
		delete arrows_particle;
		arrows_particle = nullptr;
	}
	if (map_alert_particle != nullptr)
	{
		delete map_alert_particle;
		map_alert_particle = nullptr;
	}

	return true;
}

void Selection_Panel::Handle_Input(GUI_INPUT newevent)
{
	if (inviewport == false)return;
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
			if (Selected->GetEntity() == nullptr || selected_diplomacy == ENEMY) break;
			Unit* actor = (Unit*)Selected->GetEntity();
			//Set entity target to the selected unit
			Selected->GetEntity()->GetWorker()->ResetChannel(TASK_CHANNELS::PRIMARY);
			if (UpperEntity != nullptr)
			{
				ENTITY_TYPE upper_type = UpperEntity->GetEntityType();
				DIPLOMACY upper_diplomacy = UpperEntity->GetDiplomacy();
				if (upper_diplomacy == ENEMY && selected_unit_type != MONK && selected_unit_type != WIZARD_CHMP)
				{
					if (upper_type == UNIT) actor->AddAction(App->action_manager->AttackToUnitAction(actor, (Unit*)UpperEntity));
					else if (upper_type == BUILDING) actor->AddAction(App->action_manager->AttackToBuildingAction(actor, (Building*)UpperEntity));
					App->sound->PlayFXAudio(ATTACK_SOUND);
				}
				else if (upper_type == RESOURCE && selected_unit_type == VILLAGER)
				{
					actor->AddAction(App->action_manager->RecollectAction((Villager*)actor, (Resource*)UpperEntity));
				}
				else if (upper_type == RESOURCE) {
					App->pathfinding->PushPath(actor, actor->FindSpawnCell(iPoint(mouse_x - App->render->camera.x, mouse_y - App->render->camera.y)));
					// Move Arrow
					SetMoveArrows();
				}
				else if ((selected_unit_type == WIZARD_CHMP || selected_unit_type == MONK) && upper_type == UNIT && upper_diplomacy == ALLY)
				{
					actor->AddAction(App->action_manager->HealAction(actor, (Unit*)UpperEntity));
				}
			}
			else {
				App->pathfinding->PushPath(actor, iPoint(mouse_x - App->render->camera.x, mouse_y - App->render->camera.y));
				// Move Arrow
				SetMinimapAlert(iPoint(mouse_x - App->render->camera.x, mouse_y - App->render->camera.y));
			}
		}
		else if (selected_elements.size() > 1 && selected_entity_type == UNIT) {
			{
				if (UpperEntity != nullptr)
				{
					if (UpperEntity->GetDiplomacy() == ENEMY)
					{
						if (UpperEntity->GetEntityType() == BUILDING)
						{
							App->group_move->AttackOnGroup(&(selected_elements), UpperEntity, true);
						}
						else
						{
							App->group_move->AttackOnGroup(&(selected_elements), UpperEntity, false);
						}
					}
					else if (UpperEntity->GetEntityType() == RESOURCE)
					{
						App->group_move->RecolectOnGroup(&(selected_elements), (Resource*)UpperEntity);
					}
				}
				else {
					App->group_move->GetGroupOfUnits(&(selected_elements), mouse_x - App->render->camera.x, mouse_y - App->render->camera.y, true);
					// Move Arrow
					SetMoveArrows();
				}
			}
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

void Selection_Panel::Handle_Input(UI_Element * target, GUI_INPUT input)
{
	if (target == viewport && input == MOUSE_IN) inviewport = true;
	else if (target == viewport && input == MOUSE_OUT) inviewport = false;
}

void Selection_Panel::Enable()
{
	UpperEntity = nullptr;
	action_command = nullptr;
	champions_selected = false;

	refresh_upperentity.Start();

	ResetSelectedType();
}

void Selection_Panel::Disable()
{
	group_profile.clear();
	unit_quad_selection.clear();
	building_quad_selection.clear();
	resource_quad_selection.clear();

	ResetSelectedType();
}

void Selection_Panel::Reset()
{
	Disable();
	Enable();
}

bool Selection_Panel::Load(pugi::xml_node & data)
{
	// Load Selection Panel Data ------------------------------
	// Selection Panel node
	pugi::xml_node selection_panel_data = data.child("hud_selection_panel");

	// Player Selected Entities data node
	pugi::xml_node curr_data = selection_panel_data.child("selected_type");

	// Load Selected Type
	if (curr_data != NULL)
	{
		selected_entity_type = (ENTITY_TYPE)curr_data.attribute("entity").as_int(0);
		selected_diplomacy = (DIPLOMACY)curr_data.attribute("diplomacy").as_int(0);
		selected_unit_type = (UNIT_TYPE)curr_data.attribute("unit").as_int(0);
		selected_building_type = (BUILDING_TYPE)curr_data.attribute("building").as_int(0);
		champions_selected = curr_data.attribute("champion_selected").as_bool(0);

		// Select Units
		if (selected_entity_type != NO_ENTITY)
		{
			if (selected_entity_type == UNIT)
			{
				std::list<Unit*>::const_iterator unit_start = App->entities_manager->units.begin();
				std::list<Unit*>::const_iterator unit_end = App->entities_manager->units.end();

				while (unit_start != unit_end)
				{
					if (unit_start._Ptr->_Myval->GetIfSelected() == true)
					{
						selected_elements.push_back(unit_start._Ptr->_Myval);
					}
					unit_start++;
				}
			}
			else if (selected_entity_type == BUILDING)
			{
				std::list<Building*>::const_iterator building_start = App->entities_manager->buildings.begin();
				std::list<Building*>::const_iterator building_end = App->entities_manager->buildings.end();

				while (building_start != building_end)
				{
					if (building_start._Ptr->_Myval->GetIfSelected() == true)
					{
						selected_building_type = building_start._Ptr->_Myval->GetBuildingType();
						selected_elements.push_back(building_start._Ptr->_Myval);
						break;
					}
					building_start++;
				}
			}
			else if (selected_entity_type == RESOURCE)
			{
				std::list<Resource*>::const_iterator resources_start = App->entities_manager->resources.begin();
				std::list<Resource*>::const_iterator resources_end = App->entities_manager->resources.end();

				while (resources_start != resources_end)
				{

					if (resources_start._Ptr->_Myval->GetIfSelected() == true)
					{
						selected_elements.push_back(resources_start._Ptr->_Myval);
						break;
					}
					resources_start++;
				}
			}

			//Configure Selection Panel
			if (selected_elements.size() > 1)
			{
				max_row_units = 16;
				SetGroupProfile();
				Selected->SetEntity(selected_elements.begin()._Ptr->_Myval);
			}
			else if (selected_elements.size() == 1)
			{
				Selected->SetEntity(selected_elements.begin()._Ptr->_Myval);
			}
		}
	}
	return true;
}

bool Selection_Panel::Save(pugi::xml_node & data) const
{
	// Save Selection Panel Data ------------------------------
	// Selection Panel node
	pugi::xml_node selection_panel_data = data.append_child("hud_selection_panel");

	// Player Selected Entities data node
	pugi::xml_node curr_data = selection_panel_data.append_child("selected_type");

	// Save Selected Type
	curr_data.append_attribute("entity") = selected_entity_type;
	curr_data.append_attribute("diplomacy") = selected_diplomacy;
	curr_data.append_attribute("unit") = selected_unit_type;
	curr_data.append_attribute("building") = selected_building_type;
	curr_data.append_attribute("champion_selected") = champions_selected;

	return true;
}

bool Selection_Panel::Draw()
{
	if (selected_elements.size() > 1) DrawGroup();
	else if (selected_elements.size() == 1)
	{
		Selected->UpdateStats();
		Selected->DrawProfile();
		if (Selected->GetEntity() == nullptr) selected_elements.clear();
	}

	if (arrows_particle != nullptr)
	{
		arrows_particle->Draw();

		if (arrows_particle->animation.IsEnd())
		{
			delete arrows_particle;
			arrows_particle = nullptr;
		}
	}

	if (map_alert_particle != nullptr)
	{
		SetMinimapAlertPosition();
		map_alert_particle->Draw();

		if (map_alert_particle->animation.IsEnd())
		{
			delete map_alert_particle;
			map_alert_particle = nullptr;
		}
	}

	return true;
}

void Selection_Panel::DrawGroup()
{
	SDL_Rect* box = nullptr;
	int	  life = 0, max_life = 0;

	UpdateGroup();

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
		App->render->DrawQuad({ box->x - App->render->camera.x, box->y + 36 - App->render->camera.y, 36 * life / max_life, 3 }, 0, 255, 0);
		
		//Draw icon
		group_profile[i]->Draw(App->debug_mode);

		i--;
	}
}

void Selection_Panel::DrawLife()
{
	std::list<Entity*>::const_iterator item = selected_elements.begin();
	while (item != selected_elements.end())
	{
		//Draw life
		item._Ptr->_Myval->DrawLife();
		item++;
	}
}

bool Selection_Panel::PointisInViewport(int x, int y)
{
	SDL_Point p = { x,y };
	if (SDL_PointInRect(&p, &map_viewport) == SDL_TRUE) return true;
	else return false;
}

void Selection_Panel::MoveSelectedToPoint(int x, int y)
{
	if (selected_entity_type == UNIT && selected_diplomacy == ALLY)
	{
		if (selected_elements.size() > 1)
		{
			std::list<Entity*>::iterator unit = selected_elements.begin();
			while (unit != selected_elements.end())
			{
				unit._Ptr->_Myval->GetWorker()->ResetChannel(PRIMARY);
				unit++;
			}
			App->group_move->GetGroupOfUnits(&(selected_elements), x, y, true);
		}
		else if (Selected->GetEntity() != nullptr)
		{
			Selected->GetEntity()->GetWorker()->ResetChannel(PRIMARY);
			//Selected->GetEntity()->AddAction(App->action_manager->MoveAction((Unit*)Selected->GetEntity(), iPoint(x, y)));
			App->pathfinding->PushPath((Unit*)Selected->GetEntity(), iPoint(x, y));
		}
	}
}

// Change to quads
void Selection_Panel::Select(SELECT_TYPE type)
{
	if (type == GROUP || type == DOUBLECLICK)
	{
		int x = 0, y = 0;
		uint width = 0, height = 0;

		if (type == DOUBLECLICK)
		{
			if (Selected->GetEntity() == nullptr) return;
			else if (selected_entity_type != UNIT || selected_diplomacy != ALLY) return;

			App->win->GetWindowSize(width, height);
			selection_rect = { 0, 32 , (int)width, 560 };
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
			if (((uint)selection_rect.w) < 15 && ((uint)selection_rect.h) < 15)	return;
		}

		selection_rect.x -= App->render->camera.x;
		selection_rect.y -= App->render->camera.y;


		unit_quad_selection.clear();
		App->entities_manager->units_quadtree.CollectCandidates(unit_quad_selection, selection_rect);


		int selected_amount = 0;
		if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT
			&& selected_entity_type == UNIT && selected_diplomacy == ALLY)
		{
			selected_amount = selected_elements.size();
		}
		else UnSelect_Entity();
		UNIT_TYPE u_type = NO_UNIT;
		//Select Entity
		int size = unit_quad_selection.size();
		for (int count = 0; count < size; count++)
		{
			if (unit_quad_selection[count]->GetDiplomacy() != ALLY) continue;
			else if (type == DOUBLECLICK && selected_unit_type != unit_quad_selection[count]->GetUnitType());
			else if (!unit_quad_selection[count]->GetIfSelected())
			{
				selected_amount++;
				UpperEntity = unit_quad_selection[count];
				UpperEntity->Select();
				u_type = ((Unit*)UpperEntity)->GetUnitType();
				selected_elements.push_back(UpperEntity);
				if (selected_elements.size() == 1)
				{
					ResetSelectedType(SINGLE);
				}
				else if ((type != DOUBLECLICK && selected_unit_type != NO_UNIT && champions_selected == false)
					|| (u_type == WARRIOR_CHMP || u_type == WIZARD_CHMP || u_type == ARCHER_CHMP))
				{
					ResetSelectedType(GROUP);
				}
			}
			if (selected_amount == max_selected_units) break;
		}
	}
	else
	{
		if (type == SINGLE) UnSelect_Entity();

		UpperEntity = GetUpperEntity(mouse_x, mouse_y);
		if (UpperEntity == nullptr)
		{
			if (type != ADD) ResetSelectedType();
			return;
		}
		UpperEntity->Select();


		if (type == SINGLE)
		{
			UnSelect_Entity();
			selected_elements.push_back(UpperEntity);
			ResetSelectedType(SINGLE);
		}
		else if (UpperEntity->GetEntityType() == UNIT && selected_elements.size() < max_selected_units)
		{
			if (std::find(selected_elements.begin(), selected_elements.end(), UpperEntity) == selected_elements.end())
			{
				selected_elements.push_back(UpperEntity);
				if (selected_elements.size() == 1 || selected_unit_type != NO_UNIT) ResetSelectedType(ADD);
			}
		}
	}

	//Configure Selection Panel
	if (selected_elements.size() > 1)
	{
		max_row_units = 16;
		SetGroupProfile();
		Selected->SetEntity(selected_elements.begin()._Ptr->_Myval);
		App->sound->PlayFXAudio(CLICK_SOUND);
	}
	else if (selected_elements.size() == 1)
	{
		Selected->SetEntity(selected_elements.begin()._Ptr->_Myval);
		if (Selected->GetEntity()->GetEntityType() == UNIT) App->sound->PlayFXAudio(CLICK_SOUND);
	}
	else
	{
		ResetSelectedType();
	}
}

void Selection_Panel::Select(Unit * unit)
{
	ResetSelectedType();
	selected_diplomacy = ALLY;
	selected_entity_type = UNIT;
	selected_unit_type = unit->GetUnitType();
	champions_selected = true;
	UnSelect_Entity();
	Selected->SetEntity(unit);
	selected_elements.push_back(unit);
	App->player->action_panel->SetPanelType();

}

void Selection_Panel::UpdateSelected()
{
	if (Selected->GetEntity() != nullptr)
	{
		Selected->SetEntity(Selected->GetEntity());
	}
	if (selected_elements.size() > 1) SetGroupProfile();
}

bool Selection_Panel::UpdateGroup()
{
	bool ret = false;
	std::list<Entity*>::const_iterator item = selected_elements.end();
	while (item != selected_elements.begin())
	{
		item--;
		if (item._Ptr->_Myval->GetLife() <= 0)
		{
			item._Ptr->_Myval->Deselect();
			selected_elements.remove(item._Ptr->_Myval);
			item++;
			if (item != selected_elements.end()) Selected->SetEntity(item._Ptr->_Myval);
			else Selected->SetEntity(nullptr);
			ret = true;
			continue;
		}
	}
	if (ret) 
		SetGroupProfile();
	return ret;
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
	Selected->Reset();
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

	iPoint tile = App->map->WorldToMap(x, y);
	FOG_TYPE tile_fog_type = App->fog_of_war->GetFogID(tile.x, tile.y);
	if (tile_fog_type == DARK_FOG) return nullptr;

	App->entities_manager->units_quadtree.CollectCandidates(unit_quad_selection, rect);
	App->entities_manager->buildings_quadtree.CollectCandidates(building_quad_selection, rect);
	App->entities_manager->resources_quadtree.CollectCandidates(resource_quad_selection, rect);

	// Search Units
	size = unit_quad_selection.size();
	for (count = 0; count < size; count++)
	{
		current_sprite = (Sprite*)unit_quad_selection[count]->GetAnimation()->GetCurrentSprite();
		rect = *current_sprite->GetFrame();
		pos = unit_quad_selection[count]->GetPosition();
		rect.x = (int)pos.x - current_sprite->GetXpivot();
		rect.y = (int)pos.y - current_sprite->GetYpivot();

		if (x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h)
		{
			if (ret == nullptr) ret = unit_quad_selection[count];
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
		if (building_quad_selection[count]->GetMaxLife() == 0) continue;
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

	if (ret != nullptr)
	{
		if (ret->GetEntityType() == UNIT && tile_fog_type != NO_FOG) return nullptr;
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
	if (selected_diplomacy == ALLY)
	{
		if (selected_unit_type == WARRIOR_CHMP || selected_unit_type == WIZARD_CHMP || selected_unit_type == ARCHER_CHMP)
		{
			std::list<Entity*>::const_iterator item = selected_elements.end();
			while (item != selected_elements.begin())
			{
				item--;
				if (selected_unit_type == ((Unit*)item._Ptr->_Myval)->GetUnitType())
				{
					return item._Ptr->_Myval;
				}
			}
		}
		else return Selected->GetEntity();
	}
	else return nullptr;
}

uint Selection_Panel::GetSelectedSize() const
{
	return selected_elements.size();
}

void Selection_Panel::GetSelectedType(DIPLOMACY & d_type, ENTITY_TYPE & e_type, UNIT_TYPE & u_type, BUILDING_TYPE & b_type)
{
	d_type = selected_diplomacy;
	e_type = selected_entity_type;
	u_type = selected_unit_type;
	b_type = selected_building_type;
}

void Selection_Panel::GetChampionsSelected(Unit *& warrior, Unit *& wizard, Unit *& archer)
{
	std::list<Entity*>::const_iterator item = selected_elements.end();
	UNIT_TYPE u_type = NO_UNIT;
	while (item != selected_elements.begin())
	{
		item--;
		if (item._Ptr->_Myval->GetEntityType() == UNIT)
		{
			u_type = ((Unit*)item._Ptr->_Myval)->GetUnitType();
			if (u_type == WARRIOR_CHMP) warrior = (Unit*)item._Ptr->_Myval;
			else if (u_type == WIZARD_CHMP) wizard = (Unit*)item._Ptr->_Myval;
			else if (u_type == ARCHER_CHMP) archer = (Unit*)item._Ptr->_Myval;
		}
	}
}

bool Selection_Panel::GetSelectedIsEntity()
{
	return (selected_entity_type == NO_ENTITY);
}

void Selection_Panel::ResetSelectedType(SELECT_TYPE select_type)
{
	switch (select_type)
	{
	case SINGLE: {
		selected_diplomacy = UpperEntity->GetDiplomacy();
		selected_entity_type = UpperEntity->GetEntityType();
		if (selected_entity_type == UNIT)
		{
			selected_unit_type = ((Unit*)UpperEntity)->GetUnitType();
			selected_building_type = NO_BUILDING;
		}
		else if (selected_entity_type == BUILDING)
		{
			selected_building_type = ((Building*)UpperEntity)->GetBuildingType();
			selected_unit_type = NO_UNIT;
		}
	}
				 break;
	case GROUP: {
		UNIT_TYPE u_type = ((Unit*)UpperEntity)->GetUnitType();
		if (u_type == WARRIOR_CHMP || u_type == WIZARD_CHMP || u_type == ARCHER_CHMP)
		{
			selected_unit_type = u_type;
			champions_selected = true;
		}
		else if (selected_unit_type != u_type)
			selected_unit_type = NO_UNIT;
	}
				break;
	case DOUBLECLICK: break;
	case ADD: {
		if (selected_unit_type != ((Unit*)UpperEntity)->GetUnitType()) selected_unit_type = NO_UNIT;
	}
			  break;
	default:
		break;
	}

}

void Selection_Panel::ResetSelectedType()
{
	selected_elements.clear();
	Selected->Reset();
	selected_diplomacy = NEUTRAL;
	selected_entity_type = NO_ENTITY;
	selected_unit_type = NO_UNIT;
	selected_building_type = NO_BUILDING;
	champions_selected = false;
}

void Selection_Panel::CheckSelectedType()
{
	
	UNIT_TYPE curr_type = NO_UNIT;
	champions_selected = false;
	if (selected_elements.size() > 0)
	{
		selected_unit_type = ((Unit*)selected_elements.begin()._Ptr->_Myval)->GetUnitType();
	}
	else ResetSelectedType();
	std::list<Entity*>::iterator it = selected_elements.begin();
	while(it != selected_elements.end())
	{
		curr_type = ((Unit*)it._Ptr->_Myval)->GetUnitType();
		if (curr_type == WARRIOR_CHMP || curr_type == WIZARD_CHMP || curr_type == ARCHER_CHMP)
		{
			selected_unit_type = curr_type;
			champions_selected = true;
		}
		else if (selected_unit_type != curr_type)
		{
			selected_unit_type = NO_UNIT;
		}
		it++;
	}
}

void Selection_Panel::RemoveDeadSelectedUnits(Unit* removed_unit)
{
	UNIT_TYPE removed_type = removed_unit->GetUnitType();
	removed_unit->Deselect();
	if (Selected->GetEntity() == removed_unit)
	{
		selected_elements.remove(removed_unit);
		if (selected_elements.size() > 0)
		{
			Selected->SetEntity(selected_elements.begin()._Ptr->_Myval);
		}
		else Selected->SetEntity(nullptr);
	}
	else selected_elements.remove(removed_unit);

	UpdateSelected();
	CheckSelectedType();

}

UI_Element * Selection_Panel::GetViewport()
{
	return viewport;
}

bool Selection_Panel::GetInViewport() const
{
	return inviewport;
}

bool Selection_Panel::WindowsMove()
{
	if (expand) return false;
	iPoint c_pos = { 0,0 };
	c_pos = App->map->WorldToMap(-App->render->camera.x + App->render->camera.w / 2, -App->render->camera.y + App->render->camera.h / 2);

	float speed_x = 898.13 / 2;
	float speed_y = 439.71 / 2;
	float speed = 500;

	bool ret = false;
	if (mouse_x < OFFSET_X)
	{
		if (c_pos.x == 1 && c_pos.y < 119)
		{
			App->render->camera.x += (int)SDL_ceil(speed_x * App->GetDT()/2);
			App->render->camera.y -= (int)SDL_ceil(speed_y * App->GetDT()/2);
		}
		else if (c_pos.y == 119 && c_pos.x > 1)
		{
			App->render->camera.x += (int)SDL_ceil(speed_x * App->GetDT()/2);
			App->render->camera.y += (int)SDL_ceil(speed_y * App->GetDT()/2);
		}
		else if (c_pos.y < 119 && c_pos.x >1) App->render->camera.x += (int)SDL_ceil(speed * App->GetDT());
		ret = true;

	}
	else if (mouse_x >= App->render->camera.w - OFFSET_X)
	{
		if (c_pos.x == 119 && c_pos.y > 0)
		{
			App->render->camera.x -= (int)SDL_ceil(speed_x * App->GetDT());
			App->render->camera.y += (int)SDL_ceil(speed_y * App->GetDT());
		}
		else if (c_pos.y == 0 && c_pos.x < 119)
		{
			App->render->camera.x -= (int)SDL_ceil(speed_x * App->GetDT());
			App->render->camera.y -= (int)SDL_ceil(speed_y * App->GetDT());
		}
		else if (c_pos.y > 0 && c_pos.x < 119) App->render->camera.x -= (int)SDL_ceil(speed * App->GetDT());
		ret = true;
	}
	if (mouse_y < OFFSET_Y)
	{
		if (c_pos.x == 1 && c_pos.y > 0)
		{
			App->render->camera.x -= (int)SDL_ceil(speed_x * App->GetDT()/2);
			App->render->camera.y += (int)SDL_ceil(speed_y * App->GetDT()/2);
		}
		else if (c_pos.y == 0 && c_pos.x > 1)
		{
			App->render->camera.x += (int)SDL_ceil(speed_x * App->GetDT()/2);
			App->render->camera.y += (int)SDL_ceil(speed_y * App->GetDT()/2);
		}
		else if (c_pos.y >0 && c_pos.x >1) App->render->camera.y += (int)SDL_ceil(speed * App->GetDT());
		ret = true;
	}
	else if (mouse_y > App->render->camera.h - OFFSET_Y)
	{
		if (c_pos.x == 119 && c_pos.y < 119)
		{
			App->render->camera.x += (int)SDL_ceil(speed_x * App->GetDT()/2);
			App->render->camera.y -= (int)SDL_ceil(speed_y * App->GetDT()/2);
		}
		else if (c_pos.y == 118 && c_pos.x > 1)
		{
			App->render->camera.x -= (int)SDL_ceil(speed_x * App->GetDT()/2);
			App->render->camera.y -= (int)SDL_ceil(speed_y * App->GetDT()/2);
		}
		else if (c_pos.y <119 && c_pos.x <119) App->render->camera.y -= (int)SDL_ceil(speed * App->GetDT());
		ret = true;
	}

	if (ret)App->render->CalculateCameraViewport();

	return ret;
}

void Selection_Panel::SetMoveArrows()
{
	if (arrows_particle != nullptr)
	{
		delete arrows_particle;
		arrows_particle = nullptr;
	}
	arrows_particle = new Particle(App->buff_manager->GetParticle(PARTICLE_TYPE::UI_ARROWS_PARTICLE, NO_DIRECTION));
	arrows_particle->position = iPoint(mouse_x - App->render->camera.x, mouse_y - App->render->camera.y);
	arrows_particle->animation.Reset();
}

void Selection_Panel::SetMinimapAlert(iPoint pos)
{
	if (map_alert_particle != nullptr)
	{
		delete map_alert_particle;
		map_alert_particle = nullptr;
	}
	map_alert_pos = pos;
	map_alert_particle = new Particle(App->buff_manager->GetParticle(PARTICLE_TYPE::UI_MAP_ALERT_PARTICLE, NO_DIRECTION));
	SetMinimapAlertPosition();
	map_alert_particle->animation.Reset();
}

void Selection_Panel::SetMinimapAlertPosition()
{
	iPoint new_pos = App->player->minimap_panel->MaptoMinimap(map_alert_pos.x, map_alert_pos.y);
	new_pos.y = new_pos.y / 2;
	new_pos.x -= App->render->camera.x;
	new_pos.y -= App->render->camera.y;
	map_alert_particle->position = new_pos;
}

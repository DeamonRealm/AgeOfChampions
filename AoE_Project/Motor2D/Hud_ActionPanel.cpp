#include "Hud_ActionPanel.h"

#include "j1App.h"
#include "j1Gui.h"
#include "j1Input.h"
#include "j1EntitiesManager.h"

#include "BaseEntities.h"

#include "j1ActionManager.h"
#include "Actions_Building.h"
#include "Actions_Unit.h"


#include "Hud_SelectionPanel.h"

#include "UI_Image.h"
#include "UI_Button.h"

Action_Panel_Elements::Action_Panel_Elements() 
{
	panel_icons.reserve(MAX_PANEL_CELLS);
	for (int i = 0; i < MAX_PANEL_CELLS; i++)
	{
		panel_icons[i] = { 0,0,1,1 };
	}
};

Action_Panel_Elements::~Action_Panel_Elements()
{
	panel_icons.clear();
};

void Action_Panel_Elements::AddIcon(SDL_Rect icon_rect, uint position)
{
	if (position < MAX_PANEL_CELLS)	panel_icons[position] = icon_rect;
}

void Action_Panel_Elements::ChangePanelIcons(std::vector<UI_Image*> & actual_panel) const
{
	for (uint i = 0; i < MAX_PANEL_CELLS; i++)
	{
		actual_panel[i]->ChangeTextureRect(panel_icons[i]);
	}
}

Entity * Action_Panel_Elements::GetActualEntity()
{
	return entitis_panel;
}

bool TownCenterPanel::ActivateCell(int i)
{
	if (entitis_panel == nullptr) return false;
	switch (i)
	{
	case 0: {
		entitis_panel->AddAction(App->action_manager->SpawnAction((ProductiveBuilding*)entitis_panel, VILLAGER, ALLY));
		}
		break;
	case 1: //entitis_panel->AddAction(App->action_manager->)
		break;
	case 2:
		break;
	case 3:
		break;
	case 4: 
		break;
	case 5:
		break;
	case 6:
		break;
	case 7:
		break;
	case 8: 
		break;
	case 9:
		break;
	case 10:	
		break;
	default:
		break;
	}
	return true;
}

bool UnitPanel::ActivateCell(int i)
{
	if (entitis_panel == nullptr) return false;
	switch (i)
	{
	case 0: {
		entitis_panel->SetLife(0);
		((Unit*)entitis_panel)->AddAction(App->action_manager->DieAction((Unit*)entitis_panel));
		entitis_panel = nullptr;
		}
		break;
	case 2: 
		break;
	case 3:
		break;
	case 4:
		break;
	case 5:
		break;
	case 6:
		break;
	default:
		break;
	}
	return true;
}

HeroPanel::HeroPanel() : Action_Panel_Elements() 
{
	App->gui->SetDefaultInputTarget((j1Module*)App->player);

	panel_icons[0] = { 504,441,36,36 };
	panel_icons[14] = { 0,76,36,36 };

	skill_tree = new UI_Image();
	skill_tree->SetBox({ 30, 100, 243, 345 });
	skill_tree->ChangeTextureId(CHAMPION_SKILL);
	skill_tree->ChangeTextureRect({ 0,0, 243, 345 });
	skill_tree->SetLayer(15);
	skill_tree->AdjustBox();
	skill_tree->Desactivate();

	mele_champion.reserve(6);
	mele_champion.push_back({ 243,80,36,36 });
	mele_champion.push_back({ 243,118,36,36 });
	mele_champion.push_back({ 243,156,36,36 });
	mele_champion.push_back({ 243,194,36,36 });
	mele_champion.push_back({ 243,232,36,36 });
	mele_champion.push_back({ 243,270,36,36 });


	skills.reserve(6);
	for (int i = 0; i < 6; i++)
	{
		skills.push_back(new UI_Button());
		skills[i]->SetBox({ 0, 0, 67, 61 });
		skills[i]->SetTexON(mele_champion[i], CHAMPION_SKILL);
		skills[i]->SetTexOFF(mele_champion[i], CHAMPION_SKILL);
		skills[i]->SetTexOVER(mele_champion[i], CHAMPION_SKILL);
		skills[i]->SetBoxPosition(97 + 67 * (i%2), 177 + 55*(i/2));
		skills[i]->Activate();
		skill_tree->AddChild(skills[i]);
	}

	//champion
	champion_skills_learned.reserve(3);
	champion_skills_learned.push_back(-1);
	champion_skills_learned.push_back(-1);
	champion_skills_learned.push_back(-1);
	
}

HeroPanel::~HeroPanel()
{
	skills.clear();
	
	champion_skills_learned.clear();
	mele_champion.clear();
}

bool HeroPanel::ActivateCell(int i)
{
	if (entitis_panel == nullptr) return false;
	switch (i)
	{
	case 0: {
		//open skill tree;
		}
		break;
	case 1: 
		break;
	case 2:
		break;
	case 3:
		break;
	case 4:
		break;
	case 5:
		break;
	case 6:
		break;
	case 7:
		break;
	case 8:
		break;
	case 9:
		break;
	case 10:
		break;
	default:
		break;
	}
	return true;
}

void HeroPanel::Hero_Handle_input(UI_Element * ui_element, GUI_INPUT ui_input)
{
	switch (ui_input)
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
		int x = 0;
	}
		break;
	case MOUSE_LEFT_BUTTON_REPEAT:
		break;
	case MOUSE_LEFT_BUTTON_UP:
		break;
	case MOUSE_RIGHT_BUTTON:
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


Action_Panel::Action_Panel() : action_rect({37, 624, 200, 123}), isin(false)
{
	panel_pos.x = 37;
	panel_pos.y = 624;

	panel_cells.reserve(MAX_PANEL_CELLS);

	towncenter = new TownCenterPanel();
	unitpanel = new UnitPanel();
	heropanel = new HeroPanel();

	UI_Image* cell = nullptr;
	for (int i = 0; i < MAX_PANEL_CELLS; i++)
	{
		cell = (UI_Image*)App->gui->GenerateUI_Element(IMG);
		cell->ChangeTextureRect({ 0, 0, 1, 1 });
		cell->SetBoxPosition( panel_pos.x + (i% PANEL_COLUMNS)*CELL_WIDTH, panel_pos.y + ((int)i / PANEL_COLUMNS)*CELL_HEIGHT);
		cell->ChangeTextureId(ICONS);
		panel_cells.push_back(cell);
	}
}

Action_Panel::~Action_Panel()
{
}

bool Action_Panel::CleanUp()
{
	return false;
}

bool Action_Panel::PreUpdate()
{
	App->input->GetMousePosition(mouse_pos.x, mouse_pos.y);
	SDL_Point p = { mouse_pos.x,mouse_pos.y };
	if(SDL_PointInRect(&p, &action_rect)) isin = true;
	else isin = false;

	return true;
}

bool Action_Panel::PostUpdate()
{
	return false;
}

void Action_Panel::Handle_Input(GUI_INPUT newevent)
{
	if (isin == false) return;

	switch (newevent)
	{
	case MOUSE_LEFT_BUTTON_DOWN:
		{
			cell = GetCell();
			if (actualpanel != nullptr) actualpanel->ActivateCell(cell);
		}
		break;
	case MOUSE_LEFT_BUTTON_REPEAT:
		break;
	case MOUSE_LEFT_BUTTON_UP:
		break;
	case MOUSE_IN:
		break;
	case MOUSE_OUT:
		break;
	default:
		break;
	}
}

bool Action_Panel::Draw()
{
	if (actualpanel == nullptr) return false;
	for (int count = 0; count < 1; count++)
	{
		panel_cells[count]->Draw(false);
	}
	return true;
}

int Action_Panel::GetCell() const
{
	SDL_Rect cell_clicked = {0,0,0,0};
	int ret = 0;

	for (int count = 0; count < MAX_PANEL_CELLS; count++)
	{
		cell_clicked =  { panel_pos.x + (count% PANEL_COLUMNS)*CELL_WIDTH, panel_pos.y + ((int)count / PANEL_COLUMNS)*CELL_HEIGHT,CELL_WIDTH, CELL_HEIGHT } ;
		if (SDL_PointInRect(&mouse_pos, &cell_clicked))
		{
			return count;
		}
	}

	return ret;
}

bool Action_Panel::GetIsIn() const
{
	return isin;
}

void Action_Panel::Handle_Input(UI_Element * ui_element, GUI_INPUT ui_input)
{
	if (actualpanel == heropanel)
	{
		heropanel->Hero_Handle_input(ui_element, ui_input);
	}
}

UI_Element * Action_Panel::GetHeroSkillTree() const
{
	return heropanel->skill_tree;
}

void Action_Panel::SetSelectionPanelPointer(Selection_Panel * selection_panel)
{
	player_selection_panel = selection_panel;
}

void Action_Panel::GetEntitySelected()
{
	actual_entity = player_selection_panel->GetSelected();
	if (actual_entity == nullptr)
	{
		actualpanel = nullptr;
		return;
	}
}

void Action_Panel::SetPanelType()
{
	GetEntitySelected();

	if (actual_entity == nullptr) return;

	DIPLOMACY	d_type = NEUTRAL;
	ENTITY_TYPE e_type = NO_ENTITY; 
	UNIT_TYPE u_type = NO_UNIT; 
	BUILDING_TYPE b_type = NO_BUILDING;

	player_selection_panel->GetSelectedType(d_type, e_type, u_type, b_type);

	if (d_type != ALLY)
	{
		actualpanel = nullptr;
		actual_entity = nullptr;
		return;
	}

	switch (e_type)
	{
	case NO_ENTITY: actualpanel = nullptr;
		break;
	case UNIT:
		if (u_type == VILLAGER);
		else if (u_type == WARRIOR_CHMP)
		{
			heropanel->skill_tree->Activate();
			heropanel->ChangePanelIcons(panel_cells);
			actualpanel = heropanel;
		}
		else 
		{
			for (int count = 0; count < MAX_PANEL_CELLS; count++)
			{
				unitpanel->ChangePanelIcons(panel_cells);
				actualpanel = unitpanel;
			}
		}
		break;
	case RESOURCE:
		actualpanel = nullptr;
		break;
	case BUILDING:
	{
		if (b_type == TOWN_CENTER)
		{
			towncenter->ChangePanelIcons(panel_cells);
			actualpanel = towncenter;
		}
		else actualpanel = nullptr;
	}
	break;
	default:	actualpanel = nullptr;
		break;
	}

	if (actualpanel != nullptr) actualpanel->ChangePanelTarget(actual_entity);
}

void Action_Panel::CheckSelected(int selected)
{
	if (selected == 0)
	{
		actual_entity = nullptr;
		actualpanel = nullptr;
		heropanel->skill_tree->Desactivate();
		return;
	}
	if (actualpanel != nullptr)
	{
		if (actualpanel->GetActualEntity() == nullptr)
		{
			actualpanel->ChangePanelTarget(player_selection_panel->GetSelected());
		}
	}
}
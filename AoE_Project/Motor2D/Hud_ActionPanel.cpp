#include "Hud_ActionPanel.h"

#include "j1App.h"
#include "j1Gui.h"
#include "j1Input.h"
#include "j1EntitiesManager.h"

#include "BaseEntities.h"


#include "Hud_SelectionPanel.h"

#include "UI_Image.h"


Action_Panel::Action_Panel() : action_rect({37, 624, 200, 123}), isin(false)
{
	panel_pos.x = 37;
	panel_pos.y = 624;

	panel_cells.reserve(MAX_PANEL_CELLS);

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
	case MOUSE_LEFT_BUTTON_DOWN: cell = GetCell();
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

void Action_Panel::SetPanelType(Entity * selected)
{
	if (selected == nullptr) return;
	ENTITY_TYPE selected_type = selected->GetEntityType();
	
	switch (selected_type)
	{
	case NO_ENTITY:
		break;
	case UNIT:
		for (int count = 0; count < MAX_PANEL_CELLS; count++)
		{
			panel_cells[count]->ChangeTextureRect({ 0,0,0,0 });
		}
		break;
	case RESOURCE:
		for (int count = 0; count < MAX_PANEL_CELLS; count++)
		{
			panel_cells[count]->ChangeTextureRect({ 0,0,1,1 });
		}
		break;
	case BUILDING:
		{
		BUILDING_TYPE building_type = ((Building*)selected)->GetBuildingType();
		if (building_type == TOWN_CENTER)
		{
			panel_cells[0]->ChangeTextureRect({ 576,585,36,36 });
		}
		else
		{
			for (int count = 0; count < MAX_PANEL_CELLS; count++)
			{
				panel_cells[count]->ChangeTextureRect({ 0,0,1,1 });
			}
		}
		}
		break;
	default:
		for (int count = 0; count < MAX_PANEL_CELLS; count++)
		{
			panel_cells[count]->ChangeTextureRect({ 0,0,1,1 });
		}
		break;
	}
}

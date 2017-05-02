#include "Hud_MinimapPanel.h"
#include "j1App.h"
#include "j1Gui.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Render.h"
#include "j1Player.h"
#include "j1EntitiesManager.h"

#include "Hud_SelectionPanel.h"

//UI Elements
#include "UI_Element.h"
#include "UI_Image.h"


Minimap_Panel::Minimap_Panel() : map_rect({1030,600,325,161})
{
	App->gui->SetDefaultInputTarget((j1Module*)App->player);

	minimap_background = (UI_Image*)App->gui->GenerateUI_Element(IMG);
	minimap_background->SetBox(map_rect);
	minimap_background->ChangeTextureRect({1030, 600, 325, 161});
	minimap_background->ChangeTextureId(HUD);

	map_size.x = 120;
	map_size.y = 120;

	half_tile_size.x = ((float)map_rect.w / map_size.x)* 0.5f;
	half_tile_size.y = ((float)map_rect.h / map_size.y)* 0.5f;

	int size = (map_size.x * map_size.y);
	
	minimap_cell new_cell;
	new_cell.cell_position = { map_rect.x + map_rect.w / 2, map_rect.y };

	iPoint pos;
	cells.reserve(size);
	for (int i = 0; i < size; i++)
	{
		pos = MiniMToScreen(i%map_size.x, i/map_size.y);
		cells.push_back(new_cell);
		cells[i].cell_position.x += pos.x;
		cells[i].cell_position.y += pos.y;
	}

}

Minimap_Panel::~Minimap_Panel()
{
	CleanUp();
}

bool Minimap_Panel::CleanUp()
{
	cells.clear();
	cells_to_print.clear();

	return false;
}

bool Minimap_Panel::PreUpdate()
{
	if (update_timer.Read() > MINIMAP_UPDATE_RATE)
	{
		cells_to_print.clear();
		cells_to_print.reserve(App->entities_manager->units.size());
		std::list<Unit*>::const_iterator unit = App->entities_manager->units.begin();
		std::list<Unit*>::const_iterator end = App->entities_manager->units.end();

		iPoint pos = { 0,0 };
		while (unit != end)
		{
			pos = unit._Ptr->_Myval->GetPositionRounded();
			
			pos = App->map->WorldToMap(pos.x, pos.y);
			switch (unit._Ptr->_Myval->GetDiplomacy())
			{
			case ALLY:
				cells[120 * pos.y + pos.x].cell_color = { 0,0,200,255 };
				cells_to_print.push_back(cells[120 * pos.y + pos.x]);
				break;
			case ENEMY:
				cells[120 * pos.y + pos.x].cell_color = { 200,0,0,255 };
				cells_to_print.push_back(cells[120 * pos.y + pos.x]);
				break;
			default: break;
			}
			
			unit++;
		}
		update_timer.Start();

	}
	return false;
}

bool Minimap_Panel::PostUpdate()
{
	return false;
}

bool Minimap_Panel::Draw()
{
	SDL_Color color;
	int size = cells_to_print.size();
	for (int i = 0; i < size; i++)
	{
		color = cells_to_print[i].cell_color;
		App->render->DrawQuad({ cells_to_print[i].cell_position.x, cells_to_print[i].cell_position.y,3,3 }, color.r, color.g, color.b, color.a, true, false);
	}
	return false;
}

void Minimap_Panel::Handle_Input(UI_Element * ui_element, GUI_INPUT ui_input)
{
	if (ui_element == minimap_background || (in_minimap && ui_input == MOUSE_RIGHT_BUTTON))
	{
		int x = 0, y = 0;
		App->input->GetMousePosition(x, y); 
		if (ui_input == MOUSE_LEFT_BUTTON_DOWN) MoveCameraToPoint(x, y);
		else if (ui_input == MOUSE_RIGHT_BUTTON)
			MoveUnitsToPoint(x, y);
	}
	if (ui_element == minimap_background && ui_input == MOUSE_IN)
	{
		in_minimap = true;
	}
	if (ui_element == minimap_background && ui_input == MOUSE_OUT)
	{
		in_minimap = false;
	}
}

void Minimap_Panel::Enable()
{
	update_timer.Start();
}

void Minimap_Panel::Disable()
{
}

bool Minimap_Panel::Load(pugi::xml_node & data)
{
	return false;
}

bool Minimap_Panel::Save(pugi::xml_node & data) const
{
	return false;
}

void Minimap_Panel::MoveCameraToPoint(int x, int y)
{
	if (MiniMToMap(x, y))
	{
		iPoint pos = App->map->MapToWorld(x,y);
		App->render->camera.x = -(pos.x - App->render->camera.w / 2);
		App->render->camera.y = -(pos.y - App->render->camera.h / 2);
		App->render->CalculateCameraViewport();
	}
}

void Minimap_Panel::MoveUnitsToPoint(int x, int y)
{
	if (MiniMToMap(x, y))
	{
		iPoint pos = App->map->MapToWorld(x, y);
		pos.x -= App->render->camera.w / 2;
		pos.y -= App->render->camera.h / 2;
		App->player->selection_panel->MoveSelectedToPoint(pos.x, pos.y);
	}
}

bool Minimap_Panel::MiniMToMap(int& x, int& y)
{
	fPoint rec = { (float)x,(float)y };
	rec.x -= map_rect.x + map_rect.w * 0.5;
	rec.y -= map_rect.y;

	float pX = (((rec.x / half_tile_size.x) + (rec.y / half_tile_size.y)) * 0.5f);
	float pY = (((rec.y / half_tile_size.y) - (rec.x / half_tile_size.x)) * 0.5f);

	x = (int)((pX > (floor(pX) + 0.5f)) ? ceil(pX) : floor(pX));
	y = (int)((pY > (floor(pY) + 0.5f)) ? ceil(pY) : floor(pY));

	if (x <= 0 || x >= 120 || y <= 0 || y >= 120) return false;
	else return true;
}

iPoint Minimap_Panel::MiniMToScreen(int x, int y)
{
	return iPoint((int)(x - y) * (half_tile_size.x),(int)(x + y) * (half_tile_size.y));
}

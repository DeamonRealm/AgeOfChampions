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


Minimap_Panel::Minimap_Panel() : map_rect({ 1030,600,325,161 }), minimap_size({0, 0, 325, 161})
{
	App->gui->SetDefaultInputTarget((j1Module*)App->player);

	minimap_background = (UI_Image*)App->gui->GenerateUI_Element(IMG);
	minimap_background->SetBox(map_rect);
	minimap_background->ChangeTextureRect({1030, 600, 325, 161});
	minimap_background->ChangeTextureId(HUD);
	
	map_size.x = 120;
	map_size.y = 120;

	half_tile_size.x = ((float)(map_rect.w) / map_size.x)* 0.5f;
	half_tile_size.y = ((float)(map_rect.h+2) / map_size.y)* 0.5f;

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

	// Generate Texture
	minimap_fow = SDL_CreateRGBSurface(NULL, map_rect.w, map_rect.h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_SetSurfaceBlendMode(minimap_fow, SDL_BlendMode::SDL_BLENDMODE_BLEND);
	SDL_FillRect(minimap_fow, NULL, SDL_MapRGBA(minimap_fow->format,0,0,0,255));

	update_timer.Start();
	fow_timer.Start();

	// minimap_camera move
	map_to_minimap = { (float)325 / (120*96),(float)161 / (120*47)};
	minimap_camera_size = { 0, 0, (int)((float)App->render->camera.w*map_to_minimap.x),(int)((float)App->render->camera.h*map_to_minimap.y)};
}

Minimap_Panel::~Minimap_Panel()
{
	CleanUp();
}

bool Minimap_Panel::CleanUp()
{
	if (minimap_fow_texture != nullptr) SDL_DestroyTexture(minimap_fow_texture);
	if (minimap_fow != nullptr) SDL_FreeSurface(minimap_fow);

	cells.clear();
	units_to_print.clear();
	buildings_to_print.clear();

	fow_cells_to_clear.clear();

	return false;
}

bool Minimap_Panel::PreUpdate()
{
	if (update_timer.Read() > MINIMAP_UPDATE_RATE)
	{
		units_to_print.clear();
		units_to_print.reserve(App->entities_manager->units.size());
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
				cells[120 * pos.y + pos.x].cell_color = { 20,20,255,255 };
				units_to_print.push_back(cells[120 * pos.y + pos.x]);
				break;
			case ENEMY:
				cells[120 * pos.y + pos.x].cell_color = { 255,20,20,255 };
				units_to_print.push_back(cells[120 * pos.y + pos.x]);
				break;
			default: break;
			}
			
			unit++;
		}
		update_timer.Start();

	}

	EditMinimapFoW();
	
	return false;
}

bool Minimap_Panel::PostUpdate()
{
	return false;
}

bool Minimap_Panel::Draw()
{
	App->render->DrawQuad(map_rect, 51, 151, 39, 255, true, false);

	// Draw Units in Minimap
	SDL_Color color;
	int size = units_to_print.size();
	for (int i = 0; i < size; i++)
	{
		color = units_to_print[i].cell_color;
		App->render->DrawQuad({ units_to_print[i].cell_position.x, units_to_print[i].cell_position.y,3,3 }, color.r, color.g, color.b, color.a, true, false);
	}

	// Draw Buildings in Minimap
	minimap_cell cell;
	std::list<minimap_cell>::const_iterator building = buildings_to_print.end();
	while(building != buildings_to_print.begin())
	{
		building--;
		cell = building._Ptr->_Myval;
		color = cell.cell_color;
		App->render->DrawQuad({ cell.cell_position.x, cell.cell_position.y,4,4 }, color.r, color.g, color.b, color.a, true, false);
		
	}


	// Draw FoW
	App->render->Blit(minimap_fow_texture, map_rect.x - App->render->camera.x, map_rect.y - App->render->camera.y, &minimap_size);

	minimap_background->Draw(false);
	
	// Draw Camera Viewport
	iPoint camera_pos = { (int)((float)(-App->render->camera.x + (map_size.x*96/2))*map_to_minimap.x), (int)((float)-App->render->camera.y*map_to_minimap.y)};
	App->render->DrawQuad({ map_rect.x + camera_pos.x, map_rect.y + camera_pos.y, minimap_camera_size.w, minimap_camera_size.h }, 255, 255, 255, 255, false, false);


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
	units_to_print.clear();
	units_to_print.reserve(App->entities_manager->units.size());
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
			cells[120 * pos.y + pos.x].cell_color = { 20,20,255,255 };
			units_to_print.push_back(cells[120 * pos.y + pos.x]);
			break;
		case ENEMY:
			cells[120 * pos.y + pos.x].cell_color = { 255,20,20,255 };
			units_to_print.push_back(cells[120 * pos.y + pos.x]);
			break;
		default: break;
		}

		unit++;
	}
	update_timer.Start();

	SDL_FillRect(minimap_fow, NULL, SDL_MapRGBA(minimap_fow->format, 0, 0, 0, 255));
	minimap_fow_texture = SDL_CreateTextureFromSurface(App->render->renderer, minimap_fow);

	EditMinimapFoW();

}

void Minimap_Panel::Disable()
{
	units_to_print.clear();
	buildings_to_print.clear();
	fow_cells_to_clear.clear();
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

void Minimap_Panel::SetBuildingToPrint(int x, int y, DIPLOMACY diplomacy_type)
{
	iPoint pos = App->map->WorldToMap(x, y);
 
	switch (diplomacy_type)
	{
	case ALLY:
		cells[120 * pos.y + pos.x].cell_color = { 20,20,200,255 };
		break;
	case ENEMY:
		cells[120 * pos.y + pos.x].cell_color = { 200,20,20,255 };
		break;
	default: break;
	}
	buildings_to_print.push_back(cells[120 * pos.y + pos.x]);
}

void Minimap_Panel::RemoveBuildingToPrint(int x, int y, DIPLOMACY diplomacy_type)
{
	iPoint pos = App->map->WorldToMap(x, y);

	std::list<minimap_cell>::iterator building = buildings_to_print.end();
	while (building != buildings_to_print.begin())
	{
		building--;
		if (building._Ptr->_Myval.cell_position == cells[120*pos.y+ pos.x].cell_position)
		{
			buildings_to_print.remove(building._Ptr->_Myval);
			break;
		}
	}
}

void Minimap_Panel::PushTilestoClear(uint k)
{
	fow_need_update = true;
	fow_cells_to_clear.push_back(k);
}

bool Minimap_Panel::EditMinimapFoW()
{
	if (fow_need_update && fow_timer.Read() > MINIMAP_FOW_UPDATE_RATE)
	{
		SDL_LockSurface(minimap_fow);
		
		iPoint pos = { 0,0 };
		int pixel_position = 0;
		int texture_size = minimap_size.w*minimap_size.h;
		
		
		int size = fow_cells_to_clear.size();
		for (int i = 0; i < size; i++)
		{
			pos = cells[fow_cells_to_clear[i]].cell_position;
			pos.x -= (map_rect.x);
			pos.y -= (map_rect.y);
			uint32* pixels;
			unsigned char* pixel_alpha;
			for (int j = pos.x; j < pos.x + 4; j++)
			{
				for (int k = pos.y; k < pos.y + (5 * minimap_size.w); k += minimap_size.w)
				{
					pixel_position = minimap_size.w*k + j;
					if (pixel_position < 0 || pixel_position >= texture_size) continue;
					else
					{
						pixels = (uint32*)minimap_fow->pixels;
						pixels = pixels + minimap_size.w*k + j;
						pixel_alpha = (unsigned char*)pixels + 3;
						*pixel_alpha = 0x11;
					}
				}
			}		
		}

		SDL_UnlockSurface(minimap_fow);
		if (minimap_fow_texture != nullptr) SDL_DestroyTexture(minimap_fow_texture);
		minimap_fow_texture = SDL_CreateTextureFromSurface(App->render->renderer, minimap_fow);
		fow_cells_to_clear.clear();
		fow_need_update = false;
		fow_timer.Start();
	}
	

	return false;
}

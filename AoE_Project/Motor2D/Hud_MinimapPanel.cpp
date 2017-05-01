#include "Hud_MinimapPanel.h"
#include "j1App.h"
#include "j1Gui.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Render.h"
#include "j1Player.h"

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
}

Minimap_Panel::~Minimap_Panel()
{
	CleanUp();
}

bool Minimap_Panel::CleanUp()
{
	return false;
}

bool Minimap_Panel::PreUpdate()
{
	return false;
}

bool Minimap_Panel::PostUpdate()
{
	return false;
}

bool Minimap_Panel::Draw()
{
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

	float half_width = ((float)map_rect.w/ map_size.x )* 0.5f;
	float half_height = ((float)map_rect.h / map_size.y)* 0.5f;

	float pX = (((rec.x / half_width) + (rec.y / half_height)) * 0.5f);
	float pY = (((rec.y / half_height) - (rec.x / half_width)) * 0.5f);

	x = (int)((pX > (floor(pX) + 0.5f)) ? ceil(pX) : floor(pX));
	y = (int)((pY > (floor(pY) + 0.5f)) ? ceil(pY) : floor(pY));

	if (x <= 0 || x >= 120 || y <= 0 || y >= 120) return false;
	else return true;
}

#include "j1FogOfWar.h"

#include "j1App.h"
#include "j1Map.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1EntitiesManager.h"

//Constructors ========================
j1FogOfWar::j1FogOfWar()
{
	name = "fog_of_war";
}

//Destructors =========================
j1FogOfWar::~j1FogOfWar()
{

}

//Game Loop ===========================
void j1FogOfWar::Init()
{
	active = false;
}

bool j1FogOfWar::PostUpdate()
{
	if (App->map_debug_mode)return true;

	uint size = cells_in_screen.size();
	for (uint k = 0; k < size; k++)
	{
		if(cells_in_screen[k]->alpha)App->render->FogBlit(cells_in_screen[k]->position, alpha_cell_size, cells_in_screen[k]->alpha);
	}


	if (update_timer.Read() > UPDATE_TIME)
	{
		fog_update = true;
		update_timer.Start();
	}
	else fog_update = false;



	j1Timer time;

	if (fog_update)
	{

	}
	/*
	std::vector<Unit*> units_in_screen;
	size = App->entities_manager->units_quadtree.CollectCandidates(units_in_screen, App->render->camera_viewport);
	std::vector<AlphaCell*> cells;
	for (uint k = 0; k < size; k++)
	{
		if (units_in_screen[k]->GetDiplomacy() == ALLY)
		{
			App->fog_of_war->ClearFogLayer(units_in_screen[k]->GetRenderArea(), GRAY_FOG);
			App->fog_of_war->ClearFogLayer(units_in_screen[k]->GetVision(), NO_FOG);
			App->fog_of_war->ClearAlphaLayer(units_in_screen[k]->GetVision(), 0);
		}
	}
	*/

	return true;
}

//Functionality =======================
void j1FogOfWar::GenerateFogOfWar()
{
	//Calculate alpha cells size
	alpha_cell_size = ceil(1000 / DIVISIONS_PER_PIXELS);

	//Check if alpha cell size is a multiple of 1000 to don't have pixels without fog
	while (1000 % (alpha_cell_size - MARGIN) != 0)alpha_cell_size++;

	//Calculate fog alpha layer size with the map data & alpha cells size
	alpha_layer_width = ceil((App->map->data.width * (App->map->data.tile_width + MARGIN)) / alpha_cell_size) + MARGIN;

	alpha_layer_height = ceil((App->map->data.height * (App->map->data.tile_height + MARGIN)) / (alpha_cell_size - MARGIN)) + MARGIN;

	int mid_map_lenght = ceil(App->map->data.width * (App->map->data.tile_width)) * -0.5;

	//Build fog quadtree boundaries & limit
	fog_quadtree.SetBoundaries({ (int)mid_map_lenght, 0, (int)alpha_cell_size * (int)alpha_layer_width, (int)alpha_cell_size * (int)alpha_layer_height });
	fog_quadtree.SetMaxObjects(120);
	fog_quadtree.SetDebugColor({ 255,255,0,255 });

	j1Timer time;

	//Build fog alpha layer
	//Allocate alpha layer cells
	alpha_layer = new AlphaCell[alpha_layer_width * alpha_layer_height];
	float mid_alpha_cell_size = alpha_cell_size * 0.5f;
	for (uint y = 0; y < alpha_layer_height; y++)
	{
		for (uint x = 0; x < alpha_layer_width; x++)
		{
			AlphaCell* current_cell = &alpha_layer[y * alpha_layer_width + x];

			current_cell->position = { (int)mid_map_lenght + (int)alpha_cell_size * (int)x, (int)(int)alpha_cell_size * (int)y };
			fog_quadtree.Insert(current_cell, &iPoint(current_cell->position.x + mid_alpha_cell_size, current_cell->position.y + mid_alpha_cell_size));
		}
	}

	fog_layer = new FOG_TYPE[App->map->data.width * App->map->data.height];

	for (uint y = 0; y < App->map->data.height; y++)
	{
		for (uint x = 0; x < App->map->data.width; x++)
		{
			fog_layer[y * App->map->data.width + x] = DARK_FOG;
		}
	}

	LOG("%f", time.ReadSec());
}

void j1FogOfWar::CollectFogCells()
{
	cells_in_screen.clear();
	fog_quadtree.CollectCandidates(cells_in_screen, App->render->camera_viewport);
}

FOG_TYPE j1FogOfWar::GetFogID(int x, int y) const
{
	return fog_layer[y * App->map->data.width + x];
}

std::vector<AlphaCell*> j1FogOfWar::ClearAlphaLayer(const Circle zone, unsigned short alpha)
{
	std::vector<AlphaCell*> fog_cells;
	uint size = fog_quadtree.CollectCandidates(fog_cells, zone);

	for (uint k = 0; k < size; k++)
	{
		if (fog_cells[k]->alpha > alpha)fog_cells[k]->alpha = alpha;
	}

	return fog_cells;
}

void j1FogOfWar::ClearFogLayer(const Circle zone, FOG_TYPE type)
{

	std::vector<iPoint> tiles_in;
	uint size = App->map->map_quadtree.CollectCandidates(tiles_in, zone);

	for (uint k = 0; k < size; k++)
	{
		fog_layer[tiles_in[k].y * App->map->data.width + tiles_in[k].x] = type;
	}

}


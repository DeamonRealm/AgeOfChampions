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



	if (update_timer.Read() > UPDATE_RATE)
	{
		std::vector<Unit*> units;
		uint size = App->entities_manager->units_quadtree.CollectCandidates(units, App->render->camera_viewport);
		for (uint k = 0; k < size; k++)
		{
			if (units[k]->GetAction() != WALK)entities_static_update.push_back(units[k]);
			else entities_static_update.push_back(units[k]);
		}
		update_timer.Start();

		size = cells_in_screen.size();
		for (uint k = 0; k < size; k++)
		{
			if (!cells_in_screen[k]->locked && cells_in_screen[k]->alpha < MID_ALPHA)cells_in_screen[k]->alpha = MID_ALPHA;
		}
	}

	while (!entities_release.empty())
	{
		entities_release.back()->ResetFogAround();
		entities_release.pop_back();
	}

	j1Timer time;
	std::list<Entity*> entitites_updated;
	while (time.Read() < UPDATE_TIME * 0.8 && !entities_dinamic_update.empty())
	{
		entities_dinamic_update.front()->ResetFogAround();
		entitites_updated.push_back(entities_dinamic_update.front());
		entities_dinamic_update.pop_front();
	}
	while (!entitites_updated.empty())
	{
		entitites_updated.front()->CleanFogAround();
		entitites_updated.pop_front();
	}

	j1Timer timer;
	while (timer.Read() < UPDATE_TIME && !entities_static_update.empty())
	{
		entities_static_update.back()->CheckFogAround();
		entities_static_update.pop_back();
	}

	uint size = cells_in_screen.size();
	for (uint k = 0; k < size; k++)
	{
		if(!cells_in_screen[k]->locked)App->render->FogBlit(cells_in_screen[k]->position, alpha_cell_size, cells_in_screen[k]->alpha);
		
	}

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
	uint size = fog_quadtree.CollectCandidates(cells_in_screen, App->render->camera_viewport);
	for (uint k = 0; k < size; k++)
	{
		if (!cells_in_screen[k]->locked && cells_in_screen[k]->alpha < MID_ALPHA)cells_in_screen[k]->alpha = MID_ALPHA;
	}
}

FOG_TYPE j1FogOfWar::GetFogID(int x, int y) const
{
	return fog_layer[y * App->map->data.width + x];
}

std::vector<AlphaCell*> j1FogOfWar::ClearAlphaLayer(const Circle zone, unsigned short alpha)
{
	std::vector<AlphaCell*> fog_cells;
	std::vector<AlphaCell*> definitive;

	uint size = fog_quadtree.CollectCandidates(fog_cells, zone);
	for (uint k = 0; k < size; k++)
	{
		if (!fog_cells[k]->locked)
		{
			fog_cells[k]->alpha = alpha;
			fog_cells[k]->locked = true;
			definitive.push_back(fog_cells[k]);
		}
	}

	return definitive;
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

void j1FogOfWar::CheckEntityFog(Entity * target)
{
	entities_dinamic_update.push_back(target);
}

void j1FogOfWar::ReleaseEntityFog(Entity * target)
{
	entities_release.push_back(target);
}


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
bool j1FogOfWar::PostUpdate()
{
	if (App->debug_mode)return true;

	// TODO 4:	Draw alpha layer!
	//			Collect the candidates of the fog_quadtree to get the alpha cells in camera.
	//			Iterate the filled vector and call FogBlit method from render module.
	//			The cell size is the size of the cell walls in pixels.


	// TODO 8:	Update alpha layer! 
	//			Check if there's any alpha cell in camera with a higher value than MID_ALPHA 
	// 			If theres one equal its alpha value to MID_ALPHA
	// 			More Info: https://github.com/ferranmartinvila/Fog_of_war-Research

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
	fog_quadtree.SetMaxObjects(100);
	fog_quadtree.SetDebugColor({ 255,255,0,255 });

	j1Timer time;

	//Build fog alpha layer
	//Allocate alpha layer cells
	alpha_layer = new AlphaCell[alpha_layer_width * alpha_layer_height];

	for (uint y = 0; y < alpha_layer_height; y++)
	{
		for (uint x = 0; x < alpha_layer_width; x++)
		{
			AlphaCell* current_cell = &alpha_layer[y * alpha_layer_width + x];

			current_cell->position = { (int)mid_map_lenght + (int)alpha_cell_size * (int)x, (int)(int)alpha_cell_size * (int)y };
			current_cell->alpha = ALPHA_LIMIT;
			fog_quadtree.Insert(current_cell, &iPoint(current_cell->position.x + alpha_cell_size * 0.5, current_cell->position.y + alpha_cell_size * 0.5));
		}
	}



	// TODO 1:	Create fog layer!
	//			Allocate an array of FOG_TYPE for the fog layer.
	//			Array size is the same as a map layer so get map width and map height from App->map.data
	//			Iterate the allocated array and fill it with DARK_FOG type.
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

FOG_TYPE j1FogOfWar::GetFogID(int x, int y) const
{
	// TODO 2:	Map optimization
	//			Improve this function to get the FOG_TYPE of the correct map tile
	return fog_layer[y * App->map->data.width + x];
}

void j1FogOfWar::ClearAlphaLayer(const Circle zone, unsigned short alpha)
{
	std::vector<AlphaCell*> fog_cells;
	uint size = fog_quadtree.CollectCandidates(fog_cells, zone);

	for (uint k = 0; k < size; k++)
	{
		if (fog_cells[k]->alpha > alpha)fog_cells[k]->alpha = alpha;
	}
}

void j1FogOfWar::ClearFogLayer(const Circle zone, FOG_TYPE type)
{

	std::vector<iPoint> tiles_in;
	uint size = App->map->map_quadtree.CollectCandidates(tiles_in, zone);

	for (uint k = 0; k < size; k++)
	{
		// TODO 1:	Uncomment this when TODO 1 is done!
		fog_layer[tiles_in[k].y * App->map->data.width + tiles_in[k].x] = type;
	}

}


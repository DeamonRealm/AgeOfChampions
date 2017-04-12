#include "j1FogOfWar.h"

#include "j1App.h"
#include "j1Map.h"
#include "j1Render.h"
#include "j1Window.h"

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
bool j1FogOfWar::Start()
{
	//Calculate fog size with the map data
	uint size = App->map->data.width * App->map->data.height;

	//Allocate an array of units with the size of the map
	fog_layer.reserve(size);
	
	//Initialize the array filled of dark fog
	fog_layer.assign(size, false);


	//Create a SDL_Surface for the fog
	fog_surface = SDL_CreateRGBSurface(0, App->map->data.width * App->map->data.tile_width, App->map->data.height * App->map->data.tile_height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	
	//Load fog eraser surface
	fog_eraser = IMG_Load("data/maps/fog_eraser.png");

	//Fill the fog surface with black color
	SDL_Rect rect = { 0, 0, fog_surface->w, fog_surface->h };
	if (SDL_FillRect(fog_surface, &rect, 0xff000000) != 0)
	{
		LOG("Error filling fog surface");
	}

	return true;
}

bool j1FogOfWar::PostUpdate()
{
	
	SDL_Rect fog_rect = App->render->camera;
	fog_rect.x = (fog_rect.x*-1) + (App->map->data.tile_width * App->map->data.width) * 0.5;
	fog_rect.y *= -1;
	if (SDL_BlitSurface(fog_surface, &fog_rect, App->win->screen_surface, NULL) != 0)
	{
		LOG("Error in fog surface blit: %s", SDL_GetError());
	}

	return true;
}

//Functionality =======================
uint j1FogOfWar::GetFogID(int x, int y) const
{
	return 0; // fog_layer[y * App->map->data.width + x];
}

void j1FogOfWar::ClearFogZone(Circle zone)
{

	//Collect all the tiles in the unit vision area
	iPoint pos = zone.GetPosition();
	zone.SetPosition(iPoint(pos.x - App->map->data.tile_width * 0.5f, pos.y - App->map->data.tile_height * 0.5f)); /*TEMP CASE*/

	std::vector<iPoint> tiles;
	uint size = App->map->map_quadtree.CollectCandidates(tiles, zone);

	//Clear fog in vision area
	for (uint k = 0; k < size; k++)
	{
		iPoint map_loc = App->map->WorldToMap(tiles[k].x, tiles[k].y);
		if (!fog_layer[map_loc.y * App->map->data.width + map_loc.x])
		{
			iPoint world_loc = App->map->MapToWorld(map_loc.x, map_loc.y);
			fog_layer[map_loc.y * App->map->data.width + map_loc.x] = true;
			ClearFogTile(world_loc.x, world_loc.y);
		}
	}

}

void j1FogOfWar::ClearFogTile(int in_x, int in_y)
{
	SDL_Rect sourceRect = { 0, 0, fog_eraser->w, fog_eraser->h };
	SDL_Rect destRect = { in_x, in_y, fog_eraser->w, fog_eraser->h };
	destRect.x += (App->map->data.tile_width * App->map->data.width) * 0.5;

	SDL_LockSurface(fog_surface);

	Uint32* destPixels = (Uint32*)fog_surface->pixels;
	Uint32* sourcePixels = (Uint32*)fog_eraser->pixels;

	bool keep_fog = true;



	for (uint x = 0; x < destRect.w; x++)

	{
		for (uint y = 0; y < destRect.h; y++)

		{

			Uint32* destPixel = destPixels + ((y + destRect.y) * fog_surface->w) + (destRect.x + x);
			Uint32* srcPixel = sourcePixels + ((y + sourceRect.y) * fog_eraser->w) + (sourceRect.x + x);

			unsigned char* destAlpha = (unsigned char*)destPixel + 3; // fetch alpha channel
			unsigned char* sourceAlpha = (unsigned char*)srcPixel + 3; // fetch alpha channel



			if (keep_fog && *sourceAlpha > *destAlpha)
			{
				continue;
			}

			*destAlpha = *sourceAlpha;

		}
	}

	SDL_UnlockSurface(fog_surface);

}

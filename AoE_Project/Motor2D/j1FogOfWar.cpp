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

	/*//Allocate an array of units with the size of the map
	fog_layer.reserve(size);
	
	//Initialize the array filled of dark fog
	fog_layer.assign(size, true);
	*/

	//Create a SDL_Surface for the fog
	fog_surface = SDL_CreateRGBSurface(0, App->map->data.width * App->map->data.tile_width, App->map->data.height * App->map->data.tile_height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	screen_surface = SDL_CreateRGBSurface(0, App->render->camera.w, App->render->camera.h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_SetSurfaceBlendMode(screen_surface, SDL_BlendMode::SDL_BLENDMODE_BLEND);
	//SDL_SetSurfaceAlphaMod(screen_surface, 150);
	
	//Load fog eraser surface
	fog_eraser = IMG_Load("data/maps/fog_eraser.png");
	SDL_SetSurfaceBlendMode(fog_eraser, SDL_BlendMode::SDL_BLENDMODE_BLEND);

	/*SDL_Rect rect = { 0, 0, App->map->data.width * App->map->data.tile_width, App->map->data.height * App->map->data.tile_height };
	SDL_PixelFormat* fmt = fog_surface->format;
	if (SDL_FillRect(fog_surface, &rect, SDL_MapRGBA(fmt,50,50,50,50)) != 0)
	{
		LOG("Error filling fog surface");
	}*/



	return true;
}

bool j1FogOfWar::PostUpdate()
{
	/*
	std::vector<iPoint> tiles_in_view = App->map->GetTilesInView();
	uint size = tiles_in_view.size();
	for (uint k = 0; k < size; k++)
	{
		iPoint world_loc = App->map->WorldToMap(tiles_in_view[k].x, tiles_in_view[k].y);
		if (fog_layer[world_loc.y * App->map->data.width + world_loc.x] == true)
		{
			fog_layer[world_loc.y * App->map->data.width + world_loc.x] = false;
		}
	}
	*/

	SDL_Texture* test = SDL_CreateTextureFromSurface(App->render->renderer, this->screen_surface);

	ClearFogTile(300, 300);
	
	if (SDL_RenderCopy(App->render->renderer, test, NULL, NULL))
	{
		LOG("Blit error: %s", SDL_GetError());
	}

	SDL_DestroyTexture(test);
	test = nullptr;

	/*if (SDL_BlitSurface(fog_surface, &App->render->camera, App->win->screen_surface, NULL) != 0)
	{
		LOG("Error in fog surface blit: %s", SDL_GetError());
	}
	
	SDL_SetSurfaceBlendMode(fog_surface, SDL_BLENDMODE_BLEND);
	SDL_Texture* screen_tex = SDL_CreateTextureFromSurface(App->render->renderer, App->win->screen_surface);
	SDL_SetTextureBlendMode(screen_tex, SDL_BlendMode::SDL_BLENDMODE_BLEND);
	//SDL_SetTextureAlphaMod(screen_tex, 140);
	SDL_RenderCopy(App->render->renderer, screen_tex, NULL, NULL);
	SDL_DestroyTexture(screen_tex);
	SDL_FreeSurface(App->win->screen_surface);
	*/

	return true;
}

//Functionality =======================
uint j1FogOfWar::GetFogID(int x, int y) const
{
	return 0; // fog_layer[y * App->map->data.width + x];
}

void j1FogOfWar::ClearFogZone(const Circle zone)
{

	//Collect all the tiles in the unit vision area
	std::vector<iPoint> tiles;
	uint size = App->map->map_quadtree.CollectCandidates(tiles, zone);

	//Clear fog in vision area
	for (uint k = 0; k < size; k++)
	{
		iPoint map_loc = App->map->WorldToMap(tiles[k].x, tiles[k].y);
		//fog_layer[map_loc.y * App->map->data.width + map_loc.x] = true;
		ClearFogTile(tiles[k].x, tiles[k].y);
	}

}

void j1FogOfWar::ClearFogTile(int in_x, int in_y)
{
	SDL_Rect sourceRect = { 0, 0, fog_eraser->w, fog_eraser->h };
	
	SDL_Rect destRect = { in_x, in_y, fog_eraser->w, fog_eraser->h };
	//destRect.x += (App->map->data.tile_width * App->map->data.width) * 0.5;

	SDL_LockSurface(screen_surface);

	Uint32* destPixels = (Uint32*)screen_surface->pixels;
	Uint32* sourcePixels = (Uint32*)fog_eraser->pixels;

	uint op_pix = 0;
	bool keep_fog = true;

	for (uint x = 0; x < destRect.w; x++)
	{

		for (uint y = 0; y < destRect.h; y++)
		{



			Uint32* destPixel = destPixels + ((y + destRect.y) * screen_surface->w) + (destRect.x + x);
			Uint32* srcPixel = sourcePixels + ((y + sourceRect.y) * fog_eraser->w) + (sourceRect.x + x);

			unsigned char* destAlpha = (unsigned char*)destPixel + 3; // fetch alpha channel
			unsigned char* sourceAlpha = (unsigned char*)srcPixel + 3; // fetch alpha channel

			/*if (keep_fog && *sourceAlpha > *destAlpha)
			{
				continue;
			}*/

			*destAlpha = *sourceAlpha;

		}
	}

	SDL_UnlockSurface(screen_surface);

}

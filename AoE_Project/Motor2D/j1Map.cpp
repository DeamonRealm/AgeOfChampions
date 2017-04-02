#define _CRT_SECURE_NO_WARNINGS
#include "j1Map.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1FileSystem.h"
#include "j1Textures.h"
#include "j1Window.h"
#include "j1Timer.h"
#include "j1EntitiesManager.h"

#include <math.h>

j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name = "map";
}

// Destructor
j1Map::~j1Map()
{}

// Called before render is available
bool j1Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder = (config.child("folder").child_value());

	return ret;
}

bool j1Map::CreateWalkabilityMap(uint& width, uint & height) 
{
	if (navigation_layer == nullptr)return false;
	int size = navigation_layer->width*navigation_layer->height;
	logic_map = new uchar[size];
	construction_map = new uchar[size];
	memset(logic_map, 1, size);
	for (int y = 0; y < data.height; ++y)
	{
		for (int x = 0; x < data.width; ++x)
		{
			int i = (y*navigation_layer->width) + x;

			int tile_id = navigation_layer->Get(x, y);
			TileSet* tileset = (tile_id > 0) ? GetTilesetFromTileId(tile_id) : NULL;

			if (tileset != NULL)
			{
				if (tile_id == 38)logic_map[i] = 0;	
				else logic_map[i] = 1;
			}
		}
	}
	memcpy(construction_map, logic_map, size);
	width = data.width;
	height = data.height;

	return true;
}

bool j1Map::CreateWalkCostMap(int & width, int & height, uchar ** buffer) const
{
	bool ret = false;


	uchar* map = new uchar[width*height];
	memset(map, 1, width*height);

	for (int y = 0; y < data.height; ++y)
	{
		for (int x = 0; x < data.width; ++x)
		{

			map[y*width + x] = MovementCost(x, y);

		}
	}

	*buffer = map;
	width = data.width;
	height = data.height;
	ret = true;

	return ret;
}

int j1Map::MovementCost(int x, int y) const
{
	return navigation_layer->Get(x, y);
}

void j1Map::Draw(bool debug)
{
	if (map_loaded == false) return;

	//Draw all map tiles
	uint size = data.layers.size();
	for (uint k = 0; k < size; k++)
	{
		MapLayer* layer = data.layers[k];

		if (!layer->properties.Get("Draw") && !debug)
		{
			continue;
		}

		this->map_quadtree;
		uint size = points_in_view.size();
		for (uint k = 0; k < size; k++)
		{
			//Get tile id
			int tile_id = layer->Get(points_in_view[k].x, points_in_view[k].y);
			if (tile_id == 0)continue;

			//Get tileset from tile id
			TileSet* tileset = GetTilesetFromTileId(tile_id);

			//Get tile texture rect & blit position
			SDL_Rect r = tileset->GetTileRect(tile_id);
			iPoint map_point = MapToWorld(points_in_view[k].x, points_in_view[k].y);

			//Blit the current tile
			App->render->TileBlit(tileset->texture, map_point.x, map_point.y, &r);

		}
	}

	//Draw map tiles net
	if (debug)
	{
		//Tiles size to locate the debug lines in the tiles vertex
		uint tile_h_2 = floor(data.tile_height * 0.5);
		uint tile_w = data.tile_width;

		//X axis lines
		for (uint x = 0; x < data.width; x++)
		{
			iPoint init = MapToWorld(x, 0);
			iPoint end = MapToWorld(x, data.height);

			App->render->DrawLine(init.x + tile_w, init.y + tile_h_2, end.x + tile_w, end.y + tile_h_2, 0, 250, 0);
		}

		//Y axis lines
		for (uint y = 0; y < data.height; y++)
		{
			iPoint init = MapToWorld(0, y);
			iPoint end = MapToWorld(data.width, y);

			App->render->DrawLine(init.x, init.y + tile_h_2, end.x, end.y + tile_h_2, 0, 250, 0);
		}
	}
}

int Properties::Get(const char* value, bool default_value) const
{
	std::list<Property*>::const_iterator item = list.begin();

	while (item != list.end())
	{
		if (item._Ptr->_Myval->name == value)
			return item._Ptr->_Myval->value;
		item++;
	}

	return default_value;
}

TileSet* j1Map::GetTilesetFromTileId(int id) const
{
	uint size = data.tilesets.size();
	for (uint k = 0; k < size; k++)
	{
		if (id < data.tilesets[k]->firstgid)
		{
			return data.tilesets[k-1];
		}
	}

	return data.tilesets[size - 1];
}

iPoint j1Map::MapToWorld(int x, int y) const
{
	iPoint ret;

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x * data.tile_width;
		ret.y = y * data.tile_height;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{
		ret.x = (x - y) * (int)(data.tile_width * 0.5f) - data.tile_width * 0.5f;
		ret.y = (x + y) * (int)(data.tile_height * 0.5f) + (x + y);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

iPoint j1Map::MapToWorldCenter(int x, int y)
{
	iPoint ret = MapToWorld(x,y);

	ret.x += data.tile_width * 0.5f;
	ret.y += data.tile_height * 0.5f - 1;

	return ret;
}

iPoint j1Map::WorldToMap(int x, int y) const
{
	iPoint ret(x + data.tile_width * 0.5f, y);

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = ret.x / data.tile_width;
		ret.y = y / data.tile_height;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{

		float half_width = data.tile_width * 0.5f;
		float half_height = (data.tile_height + MARGIN) * 0.5f;
	
		float pX = (((ret.x / half_width) + (ret.y / half_height)) * 0.5f);
		float pY = (((ret.y / half_height) - (ret.x / half_width)) * 0.5f);
	
		ret.x = (pX > (floor(pX) + 0.5f)) ? ceil(pX) : floor(pX);
		ret.y = (pY > (floor(pY) + 0.5f)) ? ceil(pY) : floor(pY);

		if (ret.x <= 0)ret.x = 1;
		else if (ret.x >= 120)ret.x = 119;
		if (ret.y <= 0)ret.y = 0;
		else if (ret.y >= 120)ret.y = 119;

	}
	else
	{
		LOG("Unknown map type");
	}

	return ret;
}

iPoint j1Map::WorldCenterToMap(int x, int y) const
{
	iPoint ret(0, 0);

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = (x - (data.tile_width * 0.5)) / data.tile_width;
		ret.y = (y - (data.tile_height * 0.5)) / data.tile_height;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{

		float half_width = data.tile_width * 0.5f;
		float half_height = (data.tile_height + MARGIN) * 0.5f;

		float pX = ((x - half_width) / half_width + (y - half_height) / half_height) * 0.5f;
		float pY = ((y - (half_height)) / half_height - ((x - half_width) / half_width)) * 0.5f;

		ret.x = (pX > (floor(pX) + 0.5f)) ? ceil(pX) : floor(pX);
		ret.y = (pY > (floor(pY) + 0.5f)) ? ceil(pY) : floor(pY);

	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

iPoint j1Map::FixPointMap(int x, int y)
{
	iPoint ret(x,y);

	if (ret.x < ((data.width * data.tile_width) * -0.5))
	{
		ret.x = (data.width * data.tile_width) * -0.5;
	}
	else if (ret.x >((data.width * data.tile_width) * 0.5))
	{
		ret.x = (data.width * data.tile_width) * 0.5;
	}

	float mid_map_height = (data.height * (data.tile_height + MARGIN)) * 0.5f;
	float mid_map_width = (data.width * (data.tile_width)) * 0.5f;
	float min_y = mid_map_height - ((mid_map_width - abs(ret.x)) * 0.5f);
	float max_y = mid_map_height + ((mid_map_width - abs(ret.x)) * 0.5f);

	if (y < min_y)ret.y = min_y;
	else if (y > max_y)ret.y = max_y;

	return ret;
}

void j1Map::CalculateTilesInView()
{
	points_in_view.clear();
	SDL_Rect viewport = { -App->render->camera.x - data.tile_width, -App->render->camera.y, App->render->camera.w + data.tile_width * 2, App->render->camera.h - data.tile_height * 1 };
	map_quadtree.CollectCandidates(points_in_view, viewport);
}

void j1Map::ChangeLogicMap(const iPoint & position, uint element_width, uint element_height, uint value_map)
{
	for (int i = position.y; i < position.y + element_height; i++) {
		for (int j = position.x; j < position.x + element_width; j++) {
			logic_map[i*data.width + j] = value_map;
			if (value_map == 0)
			{
				navigation_layer->data[i * data.width + j - 1] = 38;
			}
			else navigation_layer->data[i * data.width + j - 1] = 37;
		}
	}
	
}

void j1Map::ChangeConstructionMap(const iPoint & position, uint element_width, uint element_height, uint value_map)
{
	for (int i = position.y; i < position.y + element_height; i++) {
		for (int j = position.x; j < position.x + element_width; j++) {
			construction_map[i*data.width + j] = value_map;
			if (value_map == 0)
			{
				construction_layer->data[i*data.width + j - 1] = 39;
			}
			else construction_layer->data[i*data.width + j - 1] = 0;
		}
	}
}

SDL_Rect TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));
	return rect;
}

// Called before quitting
bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets
	uint size = data.tilesets.size();
	for (uint k = 0; k < size; k++)
	{
		RELEASE(data.tilesets[k]);
	}
	data.tilesets.clear();

	// Remove all layers
	size = data.layers.size();
	for (uint k = 0; k < size; k++)
	{
		RELEASE(data.layers[k]);
	}
	data.layers.clear();

	// Clean up the pugui tree
	map_file.reset();
	RELEASE_ARRAY(logic_map);
	RELEASE_ARRAY(construction_map);

	return true;
}

// Load new map
bool j1Map::Load(const char* file_name)
{
	bool ret = true;
	std::string tmp = folder + std::string(file_name);

	char* buf;
	int size = App->fs->Load(tmp.c_str(), &buf);
	pugi::xml_parse_result result = map_file.load_buffer(buf, size);

	RELEASE(buf);

	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if (ret == true)
	{
		ret = LoadMap();
	}

	// Load all tilesets info -----------------------------------------
	pugi::xml_node tileset;
	for (tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if (ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if (ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		data.tilesets.push_back(set);
	}

	// Load layer info ----------------------------------------------
	pugi::xml_node layer;
	for (layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* lay = new MapLayer();

		ret = LoadLayer(layer, lay);

		if (ret == true)
			data.layers.push_back(lay);
	}

	if (ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);


		uint size = data.tilesets.size();
		for (uint k = 0; k < size; k++)
		{
			TileSet* s = data.tilesets[k];
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.c_str(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
		}

		size = data.layers.size();
		for (uint k = 0; k < size; k++)
		{
			MapLayer* l = data.layers[k];
			LOG("Layer ----");
			LOG("name: %s", l->name.c_str());
			LOG("tile width: %d tile height: %d", l->width, l->height);
		}
	}

	map_loaded = ret;
	CalculateTilesInView();

	return ret;
}

// Load map general properties
bool j1Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if (map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();
		std::string bg_color(map.attribute("backgroundcolor").as_string());

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if (bg_color.length() > 0)
		{
			std::string red, green, blue;
			red = bg_color.substr(1, 2);
			green = bg_color.substr(3, 2);
			blue = bg_color.substr(5, 2);

			int v = 0;

			sscanf_s(red.c_str(), "%x", &v);
			if (v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.c_str(), "%x", &v);
			if (v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.c_str(), "%x", &v);
			if (v >= 0 && v <= 255) data.background_color.b = v;
		}

		std::string orientation(map.attribute("orientation").as_string());

		if (orientation == "orthogonal")
		{
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if (orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if (orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}

		uint fails = 0;
		j1Timer timer;

		//Define map area 
		SDL_Rect map_area;
		map_area.x = ((data.width) * data.tile_width) * -0.5; -App->render->camera.x;
		map_area.y = 0 - App->render->camera.y;
		map_area.w = data.width * data.tile_width;
		map_area.h = data.height * data.tile_height + (data.height);
		
		// Determine other modules quad trees map area 
		App->entities_manager->units_quadtree.SetBoundaries(map_area);
		App->entities_manager->buildings_quadtree.SetBoundaries(map_area);
		App->entities_manager->resources_quadtree.SetBoundaries(map_area);

		// Set map draw quad tree area
		map_quadtree.SetBoundaries(map_area);
		map_quadtree.SetMaxObjects(10);

		//Fill the draw quad tree with all the tiles coordinates
		for (uint y = 0; y < data.height; y++)
		{
			for (uint x = 0; x < data.width; x++)
			{
				iPoint loc = MapToWorldCenter(x, y);
				if (!map_quadtree.Insert(iPoint(x, y) , &loc)) fails++;
			}
		}

		LOG("Map QuadTree generated in: %.3f & %i errors", timer.ReadSec(), fails);
	}

	return ret;
}

bool j1Map::UnLoadMap()
{

	return data.UnLoadLayer();

}

bool j1Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name = (tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node offset = tileset_node.child("tileoffset");

	if (offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool j1Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if (image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = App->tex->Load(PATH(folder.c_str(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if (set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if (set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->num_tiles_width = set->tex_width / set->tile_width;
		set->num_tiles_height = set->tex_height / set->tile_height;
	}

	return ret;
}

bool j1Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	//Get layer name
	layer->name = node.attribute("name").as_string();
	LOG("Layer Name: %s", layer->name.c_str());
	//Get layer width
	layer->width = node.attribute("width").as_int();
	LOG("Layer Width: %u", layer->width);
	//Get layer height
	layer->height = node.attribute("height").as_int();
	LOG("Layer Height: %u", layer->height);
	//Get layer data encoding
	layer->encoding = node.child("data").attribute("encoding").as_string();
	LOG("Layer Data Encoding: %s", layer->encoding.c_str());

	//Load advanced properties
	LoadProperties(node, layer->properties);
	if (layer->properties.Get("Draw"))
	{
		draw_layer = layer;
	}
	else if (layer->properties.Get("Navigation"))
	{
		navigation_layer = layer;
	}
	else if (!layer->properties.Get("Navigation") && !layer->properties.Get("Draw"))
	{
		construction_layer = layer;
	}

	pugi::xml_node layer_data = node.child("data");

	if (layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{

		layer->data = new uint[layer->width*layer->height];
		memset(layer->data, 0, layer->width*layer->height);

		int i = 0;

		if (layer->encoding == "") {

			for (pugi::xml_node tile = layer_data.child("tile"); tile; tile = tile.next_sibling("tile"))
			{
				layer->data[i++] = tile.attribute("gid").as_int(0);
			}
		}

		else if (layer->encoding == "csv") {

			i = 0;

			std::string layer_tiles = layer_data.child_value();

			char* test = strtok((char*)layer_tiles.c_str(), ",");

			while (test != NULL) {
				test = strtok(NULL, ",");
				if (test == NULL)continue;
				if (*test == '\n')test++;
				layer->data[i++] = atoi(test);
			}

		}
	}

	return ret;
}

// Load a group of properties from a node and fill a list with it
bool j1Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
	bool ret = false;

	pugi::xml_node data = node.child("properties");

	if (data != NULL)
	{
		pugi::xml_node prop;

		for (prop = data.child("property"); prop; prop = prop.next_sibling("property"))
		{
			Properties::Property* p = new Properties::Property();

			p->name = prop.attribute("name").as_string();
			p->value = prop.attribute("value").as_bool();

			properties.list.push_back(p);
		}
	}

	return ret;
}

bool MapData::UnLoadLayer()
{

	width = height = tile_width = tile_height = NULL;
	background_color.r = 0;
	background_color.g = 0;
	background_color.b = 0;
	background_color.a = 0;
	type = MapTypes::MAPTYPE_UNKNOWN;
	tilesets.clear();
	layers.clear();

	return false;
}

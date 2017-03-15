#define _CRT_SECURE_NO_WARNINGS
#include "j1Map.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1FileSystem.h"
#include "j1Textures.h"
#include "j1Window.h"

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

bool j1Map::CreateWalkabilityMap(uint& width, uint & height, uchar** buffer) {

	bool ret = false;


	std::list<MapLayer*>::iterator item;
	for (item = data.layers.begin(); item._Ptr->_Myval != NULL; item++)
	{
		MapLayer* layer = item._Ptr->_Myval;

		if (layer->properties.Get("Navigation") == false)
			continue;

		uchar* map = new uchar[layer->width*layer->height];
		memset(map, 1, layer->width*layer->height);

		for (int y = 0; y < data.height; ++y)
		{
			for (int x = 0; x < data.width; ++x)
			{
				int i = (y*layer->width) + x;

				int tile_id = layer->Get(x, y);
				TileSet* tileset = (tile_id > 0) ? GetTilesetFromTileId(tile_id) : NULL;

				if (tileset != NULL)
				{

					if (tile_id == 29)map[i] = 0;
					else map[i] = 1;

				}
			}
		}

		*buffer = map;
		width = data.width;
		height = data.height;
		ret = true;

		break;
	}

	return ret;

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
	int ret = -1;

	if (x >= 0 && x < data.width && y >= 0 && y < data.height)
	{
		int id = data.layers.begin()._Ptr->_Next->_Myval->Get(x, y);

		ret = id;

		switch (id) {
		case 26:
			ret = 12;
			break;
		case 27:
			ret = 20;
			break;
		case 28:
			ret = 14;
			break;
		case 29:
			ret = 0;
			break;
		case 30:
			ret = 1;
			break;
		case 31:
			ret = 2;
			break;
		case 32:
			ret = 3;
			break;
		case 33:
			ret = 4;
			break;
		case 34:
			ret = 5;
			break;

		}
	}

	return ret;
}

void j1Map::Draw(bool debug)
{
	if (map_loaded == false) return;

	//Iterators to iterate all the map layers
	std::list<MapLayer*>::iterator item = data.layers.begin();
	std::list<MapLayer*>::iterator end = data.layers.end();

	//Draw all map tiles
	while(item != end)
	{
		MapLayer* layer = item._Ptr->_Myval;

		if (layer->properties.Get("Draw") == false && (layer->properties.Get("Navigation") == true && collide_layer == false))
		{
			item++;
			continue;
		}

		for (int y = 0; y < data.height; ++y)
		{
			for (int x = 0; x < data.width; ++x)
			{
				//Get tile id
				int tile_id = layer->Get(x, y);

				//Transform tile map coordinates to world coordinates
				iPoint pos = MapToWorld(x, y);

				//Check if the tile is inside the renderer view port
				if (!(pos.x + data.tile_width * 0.9 >= -App->render->camera.x && pos.x <= -App->render->camera.x + App->render->camera.w) ||
					!(pos.y + data.tile_height * 0.9 >= -App->render->camera.y && pos.y <= -App->render->camera.y + App->render->camera.h))
				{
					continue;
				}

				//Check if the tile is defined
				if (tile_id > 0)
				{
					//Get tileset from tile id
					TileSet* tileset = GetTilesetFromTileId(tile_id);

					//Get tile texture rect
					SDL_Rect r = tileset->GetTileRect(tile_id);
					
					//Blit the current tile
					App->render->Blit(tileset->texture, pos.x, pos.y, &r);
				}
			}
		}

		item++;
	}

	//Draw map tiles net
	if (debug)
	{
		//Tiles size to locate the debug lines in the tiles vertex
		uint tile_h_2 = floor(data.tile_height * 0.5);
		uint tile_w = floor(data.tile_width);

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
	std::list<TileSet*>::const_iterator item = data.tilesets.begin();
	TileSet* set = item._Ptr->_Myval;

	while (item != data.tilesets.end())
	{
		if (id < item._Ptr->_Myval->firstgid)
		{
			set = item._Ptr->_Prev->_Myval;
			break;
		}
		set = item._Ptr->_Myval;
		item++;
	}

	return set;
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
		ret.x = (x - y) * (int)(data.tile_width * 0.5f);
		ret.y = (x + y) * (int)(data.tile_height * 0.5f);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

iPoint j1Map::WorldToMap(int x, int y) const
{
	iPoint ret(0, 0);

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{

		float half_width = data.tile_width * 0.5f;
		float half_height = data.tile_height * 0.5f;
	
		float pX = ((x / half_width + y / half_height) / 2);
		float pY = ((y / half_height - (x / half_width)) / 2);
	
		pX = (pX > (floor(pX) + 0.5f)) ? ceil(pX) : floor(pX);
		pY = (pY > (floor(pY) + 0.5f)) ? ceil(pY) : floor(pY);
		ret.x = pX;
		ret.y = pY;

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
	
		if (x < ((data.width*data.tile_width) *-0.5))
			ret.x = ((data.width*data.tile_width) *-0.5);
		else if (x > ((data.width*data.tile_width)*0.5))
			ret.x = ((data.width*data.tile_width)*0.5);
		if(y<0)
			ret.y = 0;
		else if(y>(data.height*data.tile_height))
			ret.y = data.height*data.tile_height;

		return ret;
}

void j1Map::CollideLayer() {

	collide_layer = !collide_layer;

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
	std::list<TileSet*>::iterator item = data.tilesets.begin();

	while (item != data.tilesets.end())
	{
		RELEASE(item._Ptr->_Myval);
		item++;
	}
	data.tilesets.clear();

	// Remove all layers
	std::list<MapLayer*>::iterator item2 = data.layers.begin();

	while (item2 != data.layers.end())
	{
		RELEASE(item2._Ptr->_Myval);
		item2++;
	}
	data.layers.clear();

	// Clean up the pugui tree
	map_file.reset();

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

		std::list<TileSet*>::iterator item = data.tilesets.begin();
		while (item != data.tilesets.end())
		{
			TileSet* s = item._Ptr->_Myval;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.c_str(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			item++;
		}

		std::list<MapLayer*>::iterator item_layer = data.layers.begin();
		while (item_layer != data.layers.end())
		{
			MapLayer* l = item_layer._Ptr->_Myval;
			LOG("Layer ----");
			LOG("name: %s", l->name.c_str());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			item_layer++;
		}
	}

	map_loaded = ret;

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

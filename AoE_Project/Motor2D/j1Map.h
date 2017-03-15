#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2Point.h"
#include "j1Module.h"

#include "SDL/include/SDL_rect.h"
struct SDL_Texture;

// ----------------------------------------------------
struct Properties
{
	struct Property
	{
		std::string name;
		bool value;
	};

	~Properties()
	{
		std::list<Property*>::iterator item = list.begin();

		while (item != list.end())
		{
			RELEASE(item._Ptr->_Myval);
			item++;
		}

		list.clear();
	}

	int Get(const char* name, bool default_value = false) const;

	std::list<Property*>	list;
};

// ----------------------------------------------------
struct MapLayer
{
	std::string	name;
	int			width;
	int			height;
	std::string	encoding;
	uint*		data;
	Properties	properties;

	MapLayer() : data(NULL)
	{}

	~MapLayer()
	{
		RELEASE(data);
	}

	inline uint Get(int x, int y) const
	{
		return data[(y*width) + x];
	}
};

// ----------------------------------------------------
struct TileSet
{
	SDL_Rect GetTileRect(int id) const;

	std::string			name;
	int					firstgid;
	int					margin;
	int					spacing;
	int					tile_width;
	int					tile_height;
	SDL_Texture*		texture;
	int					tex_width;
	int					tex_height;
	int					num_tiles_width;
	int					num_tiles_height;
	int					offset_x;
	int					offset_y;
};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};

enum TERRAIN {

	GRASS = 27,
	WATER,
	MUD,
	DEEP_WATER,
	PORTAL

};

// ----------------------------------------------------
struct MapData
{
	uint				width;
	uint				height;
	uint				tile_width;
	uint				tile_height;
	SDL_Color			background_color;
	MapTypes			type;
	std::list<TileSet*>	tilesets;
	std::list<MapLayer*>layers;

	bool UnLoadLayer();
};

// ----------------------------------------------------
class j1Map : public j1Module
{
public:

	j1Map();

	// Destructor
	virtual ~j1Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw(bool debug);

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

private:

	//Load Map
	bool LoadMap();
	//Load Tiles Id
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	//Load Tiles Texture
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	//Load Map Layer
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	//Load Map properties
	bool LoadProperties(pugi::xml_node& node, Properties& properties);

public:

	//Class that allocate all the map data
	MapData data;

	//Unload Current map
	bool UnLoadMap();
	//Generate walkability map from map tiles id
	bool CreateWalkabilityMap(uint& width, uint & height, uchar** buffer);
	//Generate walkability cost map from map tiles id
	bool CreateWalkCostMap(int& width, int& height, uchar** buffer)const;
	//Get Movement cost from x y coordinates
	int MovementCost(int x, int y) const;
	//Get Tileset from tile id
	TileSet* GetTilesetFromTileId(int id) const;
	//Transform map coordinates to world coordinates
	iPoint MapToWorld(int x, int y) const;
	//Transform world coordinates to map coordinates
	iPoint WorldToMap(int x, int y) const;
	
	iPoint FixPointMap(int x, int y);
	//Get Collide Layer
	void CollideLayer();

private:

	pugi::xml_document	map_file;
	std::string			folder;
	bool				map_loaded;

	bool collide_layer = false;

};

#endif // __j1MAP_H__
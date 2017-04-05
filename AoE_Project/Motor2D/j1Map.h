#ifndef __j1MAP_H__
#define __j1MAP_H__

#define MARGIN 1

#include "PugiXml/src/pugixml.hpp"
#include "p2Point.h"
#include "j1Module.h"
#include "Quadtree.h"
#include "SDL/include/SDL_rect.h"
struct SDL_Texture;

// ----------------------------------------------------
struct Properties
{
	struct Property
	{
		std::string name;
		bool value = false;
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
	int			width = 0;
	int			height = 0;
	std::string	encoding;
	uint*		data = nullptr;
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
	int					firstgid = 0;
	int					margin = 0;
	int					spacing = 0;
	int					tile_width = 0;
	int					tile_height = 0;
	SDL_Texture*		texture = nullptr;
	int					tex_width = 0;
	int					tex_height = 0;
	int					num_tiles_width = 0;
	int					num_tiles_height = 0;
	int					offset_x = 0;
	int					offset_y = 0;
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
	uint					width = 0;
	uint					height = 0;
	uint					tile_width = 0;
	uint					tile_height = 0;
	SDL_Color				background_color = {255,255,255,255};
	MapTypes				type = MapTypes::MAPTYPE_UNKNOWN;
	std::vector<TileSet*>	tilesets;
	std::vector<MapLayer*>	layers;

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
	bool CreateWalkabilityMap(uint& width, uint & height);
	//Generate walkability cost map from map tiles id
	bool CreateWalkCostMap(int& width, int& height, uchar** buffer)const;
	//Get Movement cost from x y coordinates
	int MovementCost(int x, int y) const;
	//Get Tileset from tile id
	TileSet* GetTilesetFromTileId(int id) const;
	//Transform map coordinates to world coordinates
	iPoint MapToWorld(int x, int y) const;
	//Transform map coordinates to word coordinates (in tile center)
	iPoint MapToWorldCenter(int x, int y);
	//Transform world coordinates to map coordinates
	iPoint WorldToMap(int x, int y) const;
	//Transform world coordinates to map coordinates (in tile center
	iPoint WorldCenterToMap(int x, int y)const;
	//Fix an external map point inside the map limits
	iPoint FixPointMap(int x, int y);
	//Calculate the tiles that are in camera
	void CalculateTilesInView();
	//Change walkability in target tiles of the logic map
	void ChangeLogicMap(const iPoint& position, uint width, uint height,uint value_map);
	//Change walkability in target tiles of the construction map
	void ChangeConstructionMap(const iPoint& position, uint width, uint height, uint value_map);
	//Check the logic construction map in the tiles passed
	bool CheckConstructionMap(const iPoint& position, uint width, uint height)const;

	//Array with all the map logic data
	uchar*				logic_map = nullptr;
	//Array with all the map construction data
	uchar*				construction_map = nullptr;

private:
	QuadTree<iPoint>	map_quadtree;
	std::vector<iPoint> points_in_view;
	MapLayer*			draw_layer = nullptr;
	MapLayer*			navigation_layer = nullptr;
	MapLayer*			construction_layer = nullptr;
	pugi::xml_document	map_file;
	std::string			folder;
	bool				map_loaded = false;
};

#endif // __j1MAP_H__
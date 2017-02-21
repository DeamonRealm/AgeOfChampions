#include "j1LogicMap.h"



LogicMap::LogicMap(j1Map* m)
{
	if (m->CreateWalkabilityMap(width, height, &map))
		SetMap(width, height, map);
}

LogicMap::~LogicMap()
{
}
bool LogicMap::IsWalkable(int x, int y) const
{
	bool ret = false;
	iPoint pos(x, y);
	uchar t = GetTileAt(pos);
	if (t != INVALID_WALK_CODE && t > 0) {
		ret = true;
	}
	return ret;
}

// Utility: return true if pos is inside the map boundaries
bool LogicMap::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
		pos.y >= 0 && pos.y <= (int)height);
}


// Utility: return the walkability value of a tile
uchar LogicMap::GetTileAt(const iPoint& pos) const
{
	if (CheckBoundaries(pos))
		return map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}
void LogicMap::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = new uchar[width*height];
	memcpy(map, data, width*height);
}
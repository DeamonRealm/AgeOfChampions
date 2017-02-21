#pragma once
#ifndef _J1LOGIC_MAP_H_

#define INVALID_WALK_CODE 255
#include "j1Map.h"

class LogicMap
{
public:
	LogicMap(j1Map* map);

	~LogicMap();
	void SetMap(uint width, uint height, uchar* data);

	bool IsWalkable(int x, int y) const;
	bool CheckBoundaries(const iPoint& pos) const;
	uchar GetTileAt(const iPoint& pos) const;
private:
	uchar* map;
	uint width;
	uint height;
};


#endif // !_J1LOGIC_MAP_H_

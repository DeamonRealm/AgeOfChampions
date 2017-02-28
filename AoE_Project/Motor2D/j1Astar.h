#pragma once
#ifndef _J1ASTAR_H_
#define _J1ASTAR_H_
#include "j1Map.h"
#include "j1ClusterAbstraction.h"

class j1Astar
{
public:
	j1Astar();
	~j1Astar();
	void CreatePath(const uchar*logic_map,int width,int height, Node* start, Node*goal, int &distance);
private:
	int width;
	int height;
	uchar* copy_map;
};


#endif // !_J1ASTAR_H_

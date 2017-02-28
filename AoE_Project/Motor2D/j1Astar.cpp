#include "j1Astar.h"

j1Astar::j1Astar() {

}
j1Astar::~j1Astar() {

}


void j1Astar::CreatePath(const uchar*logic_map, int width_map, int height_map, Node* start, Node*goal, int &distance) {

	copy_map = new uchar[sizeof(logic_map)];
	width = width_map;
	height = height_map;

}



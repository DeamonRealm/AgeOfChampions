#include "j1Astar.h"
#include "p2Defs.h"

j1Astar::j1Astar() {

}
j1Astar::~j1Astar() {

}
bool j1Astar::IsWalkable(iPoint& destination)const
{
	bool ret = false;
	uchar t = GetTileAt(destination);
	if (t != INVALID_WALK_CODE && t > 0) {

		ret = true;

	}
	return ret;
}
// Utility: return true if pos is inside the map boundaries
bool j1Astar::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
		pos.y >= 0 && pos.y <= (int)height);
}


// Utility: return the walkability value of a tile
uchar j1Astar::GetTileAt(const iPoint& pos) const
{
	if (CheckBoundaries(pos))
		return copy_map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}
void j1Astar::SetMap(uchar * logic_map, int width_map, int height_map)
{
	RELEASE_ARRAY(copy_map);
	width = width_map;
	height = height_map;
	copy_map = new uchar[width*height];
	memcpy(copy_map, logic_map, width*height);
	
}


int j1Astar::CreatePath(Node* start, Node*goal) {
	
	int ret = -1;
	iPoint origin(start->GetPositionX(), start->GetPositionY());
	iPoint destination(goal->GetPositionX(), goal->GetPositionY());

	if (IsWalkable(origin) && IsWalkable(destination)) 
	{
		ret = 1;
		PathList open;
		PathList close;
		open.list.push_back(PathNode(0, origin.DistanceManhattan(destination), origin, nullptr));
		while (open.list.size() != 0) 
		{
			close.list.push_back(*open.GetNodeLowestScore());
			open.list.remove(*open.GetNodeLowestScore());
			if (close.list.back().pos == destination) 
			{
				std::list<PathNode>::const_iterator item = close.list.end();

				for (item--; item->parent != nullptr; item = close.Find(item->parent->pos)) 
				{
					last_path.push_back(item->pos);
				}
				last_path.push_back(close.list.begin()->pos);
				std::reverse(last_path.begin(), last_path.end());
				return ret = last_path.size();
			}
			else
			{
				PathList neightbords;
				close.list.back().FindWalkableAdjacents(neightbords,this);
				for (std::list<PathNode>::iterator  item = neightbords.list.begin(); item != neightbords.list.end(); item++) {
					if (item==close.Find(item->pos)) 
					{
						continue;
					}
					else if (item == open.Find(item->pos))
					{
						PathNode temp;
						temp = open.Find(item->pos)._Ptr->_Myval;
						item->CalculateF(destination);
						if (item->g < temp.g)
						{
							open.Find(item->pos)->parent = item->parent;
						}
					}
					else 
					{
						item->CalculateF(destination);
						open.list.push_back(item._Mynode()->_Myval);
					}
				}
				neightbords.list.clear();
			}
		}
	}
	return ret;
}

PathNode::PathNode()
{
}



PathNode::PathNode(int g, int h, const iPoint & pos, const PathNode * parent): g(g), h(h), pos(pos), parent(parent)
{
}
PathNode::PathNode(const PathNode & node) : g(node.g), h(node.h), pos(node.pos),parent(node.parent)
{
}

uint PathNode::FindWalkableAdjacents(PathList & list_to_fill, j1Astar* Astar) const
{
	iPoint cell;
	uint before = list_to_fill.list.size();
	bool northClose = false, southClose = false, eastClose = false, westClose = false;
	// south
	cell.create(pos.x, pos.y + 1);
	if (Astar->IsWalkable(cell) )
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));
	else {
		southClose = true;
	}
	// north
	cell.create(pos.x, pos.y - 1);
	if (Astar->IsWalkable(cell) )
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));
	else {
		northClose = true;
	}
	// east
	cell.create(pos.x + 1, pos.y);
	if (Astar->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));
	else {
		eastClose = true;
	}
	// west
	cell.create(pos.x - 1, pos.y);
	if (Astar->IsWalkable(cell) && cell.x != 25)
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));
	else {
		westClose = true;
	}
	// south-east
	cell.create(pos.x + 1, pos.y + 1);
	if (Astar->IsWalkable(cell) && southClose == false && eastClose == false )
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));
	// south-west
	cell.create(pos.x - 1, pos.y + 1);
	if (Astar->IsWalkable(cell) && southClose == false && westClose == false )
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));
	// north-east
	cell.create(pos.x + 1, pos.y - 1);
	if (Astar->IsWalkable(cell) && northClose == false && eastClose == false )
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));
	// north-west
	cell.create(pos.x - 1, pos.y - 1);
	if (Astar->IsWalkable(cell) && northClose == false && westClose == false )
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	
	return list_to_fill.list.size();
}

float PathNode::Score() const
{
	return g+h;
}

int PathNode::CalculateF(const iPoint & destination)
{
	switch (parent->pos.DistanceManhattan(pos))
	{
	case 1:
		g = parent->g + 10;
		break;
	case 2:
		g = parent->g + 14; // we assume here that our parent is in diagonal from us
		break;
	default:
		break;
	}

	h = pos.DistanceTo(destination) * 10;
	return  g + h;
}

bool PathNode::operator==(const PathNode & node) const
{
	return pos == node.pos;
}

bool PathNode::operator!=(const PathNode & node) const
{
	return !operator==(node);
}

std::list<PathNode>::iterator PathList::Find(const iPoint & point)
{
	std::list<PathNode>::iterator item = list.begin();
	while (item!=list.end())
	{
		if (item->pos== point) {
			return item;
		}
		++item;
	}
}

PathNode* PathList::GetNodeLowestScore() const
{
	PathNode* ret = nullptr;
	std::list<PathNode>::const_reverse_iterator item = list.crbegin();
	float min = 65535;
	while (item != list.crend())
	{
		if (item->Score() < min)
		{
			min = item->Score();
			ret = &item.base()._Ptr->_Prev->_Myval;
		}
		++item;
	}
	return ret;
}

#include "j1Pathfinding.h"

#include "j1App.h"
#include "ClusterAbstraction.h"
#include "j1Textures.h"
#include "p2Log.h"

///class Pathfinding ------------------
// Constructors =======================
j1Pathfinding::j1Pathfinding()
{

}

// Destructors ========================
j1Pathfinding::~j1Pathfinding()
{

}

//Game Loop ===========================
bool j1Pathfinding::Start()
{
	//Generate map cluster abstraction
	j1Timer timer;
	//cluster_abstraction = new ClusterAbstraction(App->map, 10);
	LOG("Cluster abstraction generated in %.3f", timer.ReadSec());

	//Load debug tiles trexture
	path_texture = App->tex->Load("maps/path_tex.png");
	return true;
}

bool j1Pathfinding::CleanUp()
{
	delete cluster_abstraction;
	return true;
}

//Functionality =======================
//Methods used during the paths creation to work with map data
bool j1Pathfinding::IsWalkable(const iPoint & destination) const
{
	//Temp while the map don't have collision mask
	return true;

	bool ret = false;
	uchar t = GetTileAt(destination);
	return (t != INVALID_WALK_CODE && t > 0);
}

bool j1Pathfinding::CheckBoundaries(const iPoint & pos) const
{
	return (pos.x >= 0 && pos.x <= (int)cluster_abstraction->width && pos.y >= 0 && pos.y <= (int)cluster_abstraction->height);
}

uchar j1Pathfinding::GetTileAt(const iPoint & pos) const
{
	if (CheckBoundaries(pos))
		return cluster_abstraction->GetValueMap(pos.x, pos.y);

	return INVALID_WALK_CODE;
}

int j1Pathfinding::CreatePath(Node * start, Node * goal)
{

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
			close.list.back().on_close = true;

			open.list.remove(*open.GetNodeLowestScore());
			if (close.list.back().pos == destination)
			{
				std::list<PathNode>::const_iterator item = close.list.end();

				for (item--; item->parent != nullptr; item = close.Find(item->parent->pos))
				{
					last_path.push_back(item->pos);
				}
				last_path.push_back(close.list.begin()->pos);

				return ret = last_path.size();
			}
			else
			{
				PathList neightbords;
				close.list.back().FindWalkableAdjacents(neightbords);
				for (std::list<PathNode>::iterator item = neightbords.list.begin(); item != neightbords.list.end(); item++) {
					if (item->on_close == true)
					{
						continue;
					}
					else if (item->on_open == true)
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
						item->on_open = true;
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

std::vector<iPoint>* j1Pathfinding::CreatePath(const iPoint & origin, const iPoint & goal)
{
	if (origin == goal)
	{
		LOG("Path origin and goal are the same!");
		return nullptr;
	}

	//Transform origin world coordinates to map tile coordinates
	iPoint origin_p(App->map->WorldToMap(origin.x, origin.y));
	//Transform goal world coordinates to map tile coordinates
	iPoint goal_p(App->map->WorldToMap(goal.x, goal.y));

	//Check if the origin/goal are walkable
	if (!IsWalkable(origin_p) || !IsWalkable(goal_p))
	{
		LOG("Path origin or goal aren't walkable!");
		return nullptr;
	}

	PathList open;
	PathList close;

	//Add origin in the closed list
	open.list.push_back(PathNode(0, origin_p.DistanceManhattan(goal_p), origin_p, nullptr));

	//Expand open list while the adjacent nodes are walkable
	while (open.list.size() != 0)
	{
		close.list.push_back(*open.GetNodeLowestScore());
		close.list.back().on_close = true;

		open.list.remove(*open.GetNodeLowestScore());

		//Check if the close list have the goal tile
		if (close.list.back().pos == goal_p)
		{

			//Build the optimal path with the visited nodes
			std::list<PathNode>::const_iterator item = close.list.end();

			//Allocate the path memory
			std::vector<iPoint>* path = new std::vector<iPoint>;
			//Add the real pixel precision goal
			path->push_back(goal);

			//Transform to world coordinates (to travel with pixels)
			for (item--; item->parent != nullptr; item = close.Find(item->parent->pos))
			{
				path->push_back(App->map->MapToWorld(item->pos.x, item->pos.y));
			}

			//Return the built path
			return path;
		}
		else
		{
			//Find current tile neighbors
			PathList neighbors;

			close.list.back().FindWalkableAdjacents(neighbors);

			for (std::list<PathNode>::iterator item = neighbors.list.begin(); item != neighbors.list.end(); item++) {

				//In case that the tile is already in the list 
				if (item->on_close == true)
				{
					continue;
				}

				//In case that the item is in the open list but not in the closed
				if (item->on_open == true)
				{
					PathNode temp;
					temp = open.Find(item->pos)._Ptr->_Myval;
					item->CalculateF(goal_p);
					if (item->g < temp.g)
					{
						open.Find(item->pos)->parent = item->parent;
					}
				}

				//In case that the item isn't in any list
				else
				{
					item->on_open = true;
					item->CalculateF(goal_p);
					open.list.push_back(item._Mynode()->_Myval);
				}
			}
			neighbors.list.clear();
		}
	}

	LOG("Path not found :(");
	return nullptr;
}
/// -----------------------------------


/// Struct PathNode -------------------
//Constructors ==============
PathNode::PathNode()
{

}

PathNode::PathNode(int g, int h, const iPoint & pos, const PathNode * parent) : g(g), h(h), pos(pos), parent(parent)
{

}

PathNode::PathNode(const PathNode & node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{

}

//Functionality =============
uint PathNode::FindWalkableAdjacents(PathList& list_to_fill) const
{
	iPoint cell;
	uint before = list_to_fill.list.size();
	bool northClose = false, southClose = false, eastClose = false, westClose = false;
	// south
	cell.create(pos.x, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));
	else {
		southClose = true;
	}
	// north
	cell.create(pos.x, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));
	else {
		northClose = true;
	}
	// east
	cell.create(pos.x + 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));
	else {
		eastClose = true;
	}
	// west
	cell.create(pos.x - 1, pos.y);
	if (App->pathfinding->IsWalkable(cell) && cell.x != 25)
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));
	else {
		westClose = true;
	}
	// south-east
	cell.create(pos.x + 1, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell) && southClose == false && eastClose == false)
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));
	// south-west
	cell.create(pos.x - 1, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell) && southClose == false && westClose == false)
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));
	// north-east
	cell.create(pos.x + 1, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell) && northClose == false && eastClose == false)
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));
	// north-west
	cell.create(pos.x - 1, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell) && northClose == false && westClose == false)
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));


	return list_to_fill.list.size();
}

float PathNode::Score() const
{
	return g + h;
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

//Operators =================
bool PathNode::operator==(const PathNode & node) const
{
	return pos == node.pos;
}

bool PathNode::operator!=(const PathNode & node) const
{
	return !operator==(node);
}
/// -----------------------------------


///Struct PathList --------------------
//Functionality =============
std::list<PathNode>::iterator PathList::Find(const iPoint & point)
{
	std::list<PathNode>::iterator item = list.begin();
	while (item != list.end())
	{
		if (item->pos == point) {
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
/// -----------------------------------
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

void j1Pathfinding::SetMap(uint width, uint height, uchar * data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(logic_map);
	logic_map = new uchar[width*height];
	memcpy(logic_map, data, width*height);
}

void j1Pathfinding::InitClusterAbstraction()
{
	cluster_abstraction = new ClusterAbstraction(App->map, 10);
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
		return GetValueMap(pos.x, pos.y);

	return INVALID_WALK_CODE;
}

uchar j1Pathfinding::GetValueMap(int x, int y) const
{
	return logic_map[(y*width) + x];
}

int j1Pathfinding::CreatePath(Node * start, Node * goal)
{

	int ret = -1;
	iPoint origin(start->GetPositionX(), start->GetPositionY());
	iPoint destination(goal->GetPositionX(), goal->GetPositionY());

	if (IsWalkable(origin) && IsWalkable(destination))
	{
		ret = 1;
		OpenList open;
		PathList close;
		
		open.list.push(PathNode(0, origin.DistanceManhattan(destination), origin, nullptr));
		while (open.list.size() != 0)
		{
			close.list.push_back(open.list.top());
			close.list.back().on_close = true;
			open.list.pop();
			if (close.list.back().pos == destination)
			{
				std::list<PathNode>::const_iterator item = close.list.end();
				last_path.clear();
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
					//	temp = open.Find(item->pos)._Ptr->_Myval;
						item->CalculateF(destination);
						if (item->g < temp.g)
						{
					//		open.Find(item->pos)->parent = item->parent;
						}
					}
					else
					{
						item->on_open = true;
						item->CalculateF(destination);
						open.list.push(item._Mynode()->_Myval);
					}
				}
				neightbords.list.clear();
			}
		}
	}
	return ret;
}

std::vector<iPoint>* j1Pathfinding::CreatePath(const iPoint & origin, const iPoint & destination)
{
	iPoint map_origin = App->map->WorldToMap(origin.x, origin.y);
	iPoint map_goal = App->map->WorldToMap(destination.x, destination.y);
	SetMap(cluster_abstraction->width, cluster_abstraction->height, cluster_abstraction->logic_map);

	if (!IsWalkable(map_origin)||!IsWalkable(map_goal))
	{
		return nullptr;
	}

	Node* start = nullptr;
	Node* goal = nullptr;

	start = cluster_abstraction->PutNode(map_origin);
	goal = cluster_abstraction->PutNode(map_goal);
	if (start->GetClusterId() == goal->GetClusterId()) 
	{
		CreatePath(start, goal);
		std::vector<iPoint>* path = new std::vector<iPoint>;

		for (int i = 0; i < last_path.size(); i++) 
		{
			path->push_back(App->map->MapToWorld(last_path[i].x, last_path[i].y));
		}
		
		//std::reverse(path->begin(), path->end());

		//Return the built path
		return path;
	}
	else 
	{
		int bfs = cluster_abstraction->CreateBFS(start, goal);
		if (bfs != -1)
		{
			std::vector<iPoint>* path = new std::vector<iPoint>;
			int best_path_size = cluster_abstraction->GetBestPath().size();
			int node_pos2 = best_path_size - 1;
			int node_pos1 = node_pos2-1;

			while (node_pos1 != 0)
			{
				Node* start= cluster_abstraction->GetBestPathAt(node_pos1);
				Node* goal = cluster_abstraction->GetBestPathAt(node_pos2);
				if (start->GetClusterId() != goal->GetClusterId())
				{

				}
				else
				{
					CreatePath(start, goal);
					for (int i = 0; i < last_path.size(); i++)
					{
						if (path->size() == 0) {
							path->push_back(App->map->MapToWorld(last_path[i].x, last_path[i].y));
						}
						else if (path->back() != last_path[i])
						{
							path->push_back(App->map->MapToWorld(last_path[i].x, last_path[i].y));
						}
					
					}

				}		


				node_pos1--;
				node_pos2--;

			}
			cluster_abstraction->DeleteNode(start, goal);
			//std::reverse(path->begin(), path->end());
			return path;
		}
	}
	
	//get 2 nodes if they are in the same cluster get the cluster map
	//if not are in the same cluster set node2 parent of node1
}

std::vector<iPoint>* j1Pathfinding::SimpleAstar(const iPoint & origin, const iPoint & destination)
{

	int ret = -1;
	iPoint map_origin = App->map->WorldToMap(origin.x, origin.y);
	iPoint map_goal = App->map->WorldToMap(destination.x, destination.y);
	if (IsWalkable(map_origin) && IsWalkable(map_goal))
	{
		ret = 1;
		OpenList open;
		PathList close;

		open.list.push(PathNode(0, map_origin.DistanceManhattan(map_goal), map_origin, nullptr));
		while (open.list.size() != 0)
		{
			close.list.push_back(open.list.top());
			close.list.back().on_close = true;
			open.list.pop();
			if (close.list.back().pos == map_goal)
			{
				std::list<PathNode>::const_iterator item = close.list.end();
				std::vector<iPoint>* path = new std::vector<iPoint>;

				last_path.clear();
				for (item--; item->parent != nullptr; item = close.Find(item->parent->pos))
				{
					last_path.push_back(item->pos);
					path->push_back(App->map->MapToWorld(item->pos.x, item->pos.y));
					
				}
				last_path.push_back(close.list.begin()->pos);
				path->push_back(App->map->MapToWorld(item->pos.x, item->pos.y));

				return path;
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
						//	temp = open.Find(item->pos)._Ptr->_Myval;
						item->CalculateF(map_goal);
						if (item->g < temp.g)
						{
							//		open.Find(item->pos)->parent = item->parent;
						}
					}
					else
					{
						item->on_open = true;
						item->CalculateF(map_goal);
						open.list.push(item._Mynode()->_Myval);
					}
				}
				neightbords.list.clear();
			}
		}
	}
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
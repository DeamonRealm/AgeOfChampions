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
	RELEASE_ARRAY(logic_map);
	RELEASE_ARRAY(path_nodes);
	return true;
}

void j1Pathfinding::SetMap(uint width, uint height, uchar *& data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(logic_map);
	RELEASE_ARRAY(path_nodes);
	int size = width*height;
	logic_map = data;
	path_nodes = new PathNode[size];

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
	bool ret = false;
	uchar t = GetTileAt(destination);
	return (t != INVALID_WALK_CODE && t > 0);
}

bool j1Pathfinding::CheckBoundaries(const iPoint & pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width && pos.y >= 0 && pos.y <= (int)height);
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




PathNode* j1Pathfinding::GetPathNode(int x, int y)
{
	return &path_nodes[(y*width) + x];
}

int j1Pathfinding::CreatePath(Node * start, Node * goal)
{
	int ret = -1;


	iPoint map_origin(start->GetPositionX(), start->GetPositionY());
	iPoint map_goal(goal->GetPositionX(), goal->GetPositionY());

	if (IsWalkable(map_origin) && IsWalkable(map_goal))
	{
		if (IsWalkable(map_origin) && IsWalkable(map_goal))
		{
			ret = 1;
			OpenList open;
			PathNode* firstNode = GetPathNode(map_origin.x, map_origin.y);
			firstNode->SetPosition(map_origin);
			firstNode->g = 0;
			firstNode->h = map_origin.DistanceOctile(map_goal);

			open.queue.push(firstNode);
			PathNode* current = nullptr;
			while (open.queue.size() != 0)
			{
				current = open.queue.top();
				open.queue.top()->on_close = true;
				open.queue.pop();
				if (current->pos == map_goal)
				{
					last_path.clear();
					for (; current->parent != nullptr; current = GetPathNode(current->parent->pos.x, current->parent->pos.y))
					{
						last_path.push_back(current->pos);
					}
					last_path.push_back(current->pos);
					return last_path.size();
				}
				else
				{
					PathList neightbords;
					current->FindWalkableAdjacents(&neightbords);
					for (std::list<PathNode*>::iterator item = neightbords.list.begin(); item != neightbords.list.end(); item++) {
						PathNode* temp = item._Mynode()->_Myval;

						if (temp->on_close == true)
						{
							continue;
						}
						else if (temp->on_open == true)
						{
							int last_g_value = temp->g;
							temp->CalculateF(map_goal);
							if (last_g_value < temp->g)
							{
								temp->parent = GetPathNode(current->pos.x, current->pos.y);
							}
							else {
								temp->g = last_g_value;
							}
						}
						else
						{
							temp->on_open = true;
							temp->CalculateF(map_goal);
							open.queue.push(temp);
						}
					}

					neightbords.list.clear();
				}
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
	

	if (!IsWalkable(map_origin) || !IsWalkable(map_goal) || map_origin == map_goal);
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
	
	int size = width*height;
	std::fill(path_nodes, path_nodes + size, PathNode(-1, -1, iPoint(-1, -1), nullptr));

	int ret = -1;
	iPoint mouse_position = App->map->FixPointMap(destination.x, destination.y);
	iPoint map_origin = App->map->WorldToMap(origin.x, origin.y);
	iPoint map_goal = App->map->WorldToMap(mouse_position.x, mouse_position.y);
	if (map_origin == map_goal)
	{
		std::vector<iPoint>* path = new std::vector<iPoint>;
		path->push_back(mouse_position);
		return path;
	}
	LOG("ORIGIN WORLD: X=%i Y=%i		ORIGIN MAP: X=%i Y=%i", origin.x, origin.y, map_origin.x, map_origin.y);
	LOG("GOAL WORLD: X=%i Y=%i			GOAL MAP: X=%i Y=%i", destination.x, destination.y, map_goal.x, map_goal.y);
	if (IsWalkable(map_origin) && IsWalkable(map_goal))
	{
		ret = 1;
		OpenList open;
		PathNode* firstNode = GetPathNode(map_origin.x, map_origin.y);
		firstNode->SetPosition(map_origin);
		firstNode->g = 0;
		firstNode->h= map_origin.DistanceOctile(map_goal);

		open.queue.push(firstNode);
		PathNode* current=nullptr;
		while (open.queue.size() != 0)
		{
			current = open.queue.top();
			open.queue.top()->on_close = true;
			open.queue.pop();
			if (current->pos == map_goal)
			{
				
				//std::list<PathNode>::const_iterator item = current;
				std::vector<iPoint>* path = new std::vector<iPoint>;
				last_path.clear();
				path->push_back(mouse_position);

				for (; current->parent != nullptr; current = GetPathNode(current->parent->pos.x, current->parent->pos.y))
				{
					last_path.push_back(current->pos);
					path->push_back(App->map->MapToWorld(current->pos.x, current->pos.y));
					
				}
				last_path.push_back(current->pos);
				path->push_back(origin);
				
				return path;
			}
			else
			{
				PathList neightbords;
				current->FindWalkableAdjacents(&neightbords);
				for (std::list<PathNode*>::iterator item = neightbords.list.begin(); item != neightbords.list.end(); item++) {
					PathNode* temp = item._Mynode()->_Myval;

					if (temp->on_close == true)
					{
						continue;
					}
					else if (temp->on_open == true)
					{
						int last_g_value = temp->g;
						temp->CalculateF(map_goal);
						if (last_g_value <temp->g)
						{
							temp->parent = GetPathNode(current->pos.x,current->pos.y);
						}
						else {
							temp->g = last_g_value;
						}
					}
					else
					{
						temp->on_open = true;
						temp->CalculateF(map_goal);
						open.queue.push(temp);

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

PathNode::PathNode(int g, int h, const iPoint & pos, const PathNode * parent) : g(g), h(h), pos(pos), parent(parent),on_close(false),on_open(false)
{

}

PathNode::PathNode(const PathNode & node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{

}

//Functionality =============
uint PathNode::FindWalkableAdjacents(PathList* list_to_fill) const
{
	iPoint cell;
	uint before = list_to_fill->list.size();
	bool northClose = false, southClose = false, eastClose = false, westClose = false;
	// south
	cell.create(pos.x, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
	{
		PathNode* node = App->pathfinding->GetPathNode(cell.x, cell.y);
		if (node->pos != cell) {
			node->parent = this;
			node->pos = cell;
		}
		list_to_fill->list.push_back(node);
	}
	else
	{
		southClose = true;
	}
	// north
	cell.create(pos.x, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
	{
		PathNode* node = App->pathfinding->GetPathNode(cell.x, cell.y);
		if (node->pos != cell) {
			node->parent = this;
			node->pos = cell;
		}
		list_to_fill->list.push_back(node);
	}
	else
	{
		northClose = true;
	}
	// east
	cell.create(pos.x + 1, pos.y);
	if (App->pathfinding->IsWalkable(cell)) 
	{
		PathNode* node = App->pathfinding->GetPathNode(cell.x, cell.y);
		if (node->pos != cell) {
			node->parent = this;
			node->pos = cell;
		}
		list_to_fill->list.push_back(node);
	}
	else
	{
		eastClose = true;
	}
	// west
	cell.create(pos.x - 1, pos.y);
	if (App->pathfinding->IsWalkable(cell)) 
	{
		PathNode* node = App->pathfinding->GetPathNode(cell.x, cell.y);
		if (node->pos != cell) {
			node->parent = this;
			node->pos = cell;
		}
		list_to_fill->list.push_back(node);
	}
	else
	{
		westClose = true;
	}
	// south-east
	cell.create(pos.x + 1, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell) && southClose == false && eastClose == false) 
	{
		PathNode* node = App->pathfinding->GetPathNode(cell.x, cell.y);
		if (node->pos != cell) {
			node->parent = this;
			node->pos = cell;
		}
		list_to_fill->list.push_back(node);
	}
	// south-west
	cell.create(pos.x - 1, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell) && southClose == false && westClose == false) 
	{
		PathNode* node = App->pathfinding->GetPathNode(cell.x, cell.y);
		if (node->pos != cell) {
			node->parent = this;
			node->pos = cell;
		}
		list_to_fill->list.push_back(node);
	}
	// north-east
	cell.create(pos.x + 1, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell) && northClose == false && eastClose == false)
	{
		PathNode* node = App->pathfinding->GetPathNode(cell.x, cell.y);
		if (node->pos != cell) {
			node->parent = this;
			node->pos = cell;
		}
		list_to_fill->list.push_back(node);
	}
	// north-west
	cell.create(pos.x - 1, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell) && northClose == false && westClose == false) 
	{
		PathNode* node = App->pathfinding->GetPathNode(cell.x, cell.y);
		if (node->pos != cell) {
			node->parent = this;
			node->pos = cell;
		}
		list_to_fill->list.push_back(node);
	}


	return list_to_fill->list.size();
}

float PathNode::Score() const
{
	return g + h;
}

int PathNode::CalculateF(const iPoint & destination)
{
	/*switch (parent->pos.DistanceOctile(pos))
	{
	case 10:
		g = parent->g + 10;
		break;
	case 14:
		g = parent->g + 14; // we assume here that our parent is in diagonal from us
		break;
	default:
		g = parent->g + 14; // we assume here that our parent is in diagonal from us

		break;
	}
	*/
	g = parent->g + parent->pos.DistanceOctile(pos);
	h = pos.DistanceOctile(destination) * 10;
	return  g + h;
}

void PathNode::SetPosition(const iPoint & value)
{
	pos = value;
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
/*
std::list<PathNode>::iterator PathList::Find(const iPoint & point)
{
	std::list<PathNode*>::iterator item = list.begin();
	while (item != list.end())
	{
		if (item->pos == point) {
			return item;
		}
		++item;
	}
	
}
*/
/*
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
*/
/// -----------------------------------
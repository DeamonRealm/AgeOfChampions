#include "j1Pathfinding.h"
#include "j1Map.h"
#include "j1App.h"
#include "j1Textures.h"
#include "Actions_Unit.h"
#include "p2Log.h"

///class Pathfinding ------------------
// Constructors =======================
j1Pathfinding::j1Pathfinding()
{
	name = "pathfinding";
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

bool j1Pathfinding::PreUpdate()
{
	if (to_path.empty())return true;

	pathTime.Start();
	std::list<ToPath>::const_iterator unit_it = to_path.begin();

	while (!to_path.empty())
	{
		if (pathTime.Read() > MAX_PATH_TIME) return true;
		ToPath current_unit = unit_it._Ptr->_Myval;
		if (current_unit.unit != nullptr) 
		{
			if (current_unit.unit->GetAction() != DIE || current_unit.unit->GetAction() != DISAPPEAR)
			{
				std::vector<iPoint>* path = SimpleAstar(current_unit.unit->GetPositionRounded(), current_unit.destination);
				if (current_unit.priority)
					current_unit.unit->AddPriorizedAction((Action*)App->action_manager->MoveAction(path, current_unit.unit, current_unit.target));
				else
					current_unit.unit->AddAction((Action*)App->action_manager->MoveAction(path, current_unit.unit, current_unit.target));
			}
		}
		unit_it++;
		to_path.pop_front();
	}

	return true;
}

bool j1Pathfinding::CleanUp()
{
	to_path.clear();
	App->tex->UnLoad(path_texture);
	if(path_nodes != nullptr)	RELEASE_ARRAY(path_nodes);
	return true;
}

void j1Pathfinding::SetMap(uint width, uint height)
{
	this->width = width;
	this->height = height;
	map_min_x = 0;
	map_min_y = 0;
	map_max_x = width;
	map_max_y = height;
	RELEASE_ARRAY(path_nodes);
	int size = width*height;
	path_nodes = new PathNode[size];

}

void j1Pathfinding::SetMapLimits(int position_x, int position_y, int width, int height)
{
	map_min_x = position_x;
	map_min_y = position_y;
	map_max_x = position_x + width;
	map_max_y = position_y + height;
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
	return (pos.x >= map_min_x && pos.x < map_max_x && pos.y >= map_min_y && pos.y < map_max_y);
}

uchar j1Pathfinding::GetTileAt(const iPoint & pos) const
{
	if (CheckBoundaries(pos))
		return GetValueMap(pos.x, pos.y);

	return INVALID_WALK_CODE;
}

uchar j1Pathfinding::GetValueMap(int x, int y) const
{
	return App->map->logic_map[(y*width) + x];
}




PathNode* j1Pathfinding::GetPathNode(int x, int y)
{
	return &path_nodes[(y*width) + x];
}



std::vector<iPoint>* j1Pathfinding::SimpleAstar(const iPoint& origin, const iPoint& destination)
{

	int size = width*height;
	std::fill(path_nodes, path_nodes + size, PathNode(-1, -1, iPoint(-1, -1), nullptr));

	int ret = -1;

	iPoint mouse_position = App->map->FixPointMap(destination.x, destination.y);
	iPoint map_origin = App->map->WorldToMap(origin.x, origin.y);
	iPoint map_goal = App->map->WorldToMap(mouse_position.x, mouse_position.y);

	iPoint diference = origin - App->map->MapToWorld(map_origin.x, map_origin.y);

	if (map_origin == map_goal)
	{
		std::vector<iPoint>* path = new std::vector<iPoint>;
		path->push_back(mouse_position);
		return path;
	}

	/*LOG("ORIGIN WORLD: X=%i Y=%i		ORIGIN MAP: X=%i Y=%i", origin.x, origin.y, map_origin.x, map_origin.y);
	LOG("GOAL WORLD: X=%i Y=%i			GOAL MAP: X=%i Y=%i", destination.x, destination.y, map_goal.x, map_goal.y);*/

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
			if (open.queue.size() > 1000)
			{
				return nullptr;
			}
			current = open.queue.top();
			open.queue.top()->on_close = true;
			open.queue.pop();
			if (current->pos == map_goal)
			{

				std::vector<iPoint>* path = new std::vector<iPoint>;

				path->push_back(mouse_position);
				iPoint mouse_cell = App->map->WorldToMap(mouse_position.x, mouse_position.y);
				if (mouse_cell == current->pos)
					current = GetPathNode(current->parent->pos.x, current->parent->pos.y);
				iPoint current_world;
				for (; current->parent != nullptr; current = GetPathNode(current->parent->pos.x, current->parent->pos.y))
				{

					current_world = App->map->MapToWorld(current->pos.x, current->pos.y);
					current_world.x += diference.x;
					current_world.y += diference.y;
					path->push_back(current_world);

				}

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

	return nullptr;
}

void j1Pathfinding::PushPath(Unit * unit, iPoint destination, iPoint target, bool priority, TASK_CHANNELS task)
{
	ToPath getUnit(unit, destination,target, priority,task);
	to_path.remove(getUnit);
	to_path.push_back(getUnit);
}

/// -----------------------------------


/// Struct PathNode -------------------
//Constructors ==============
PathNode::PathNode()
{

}

PathNode::PathNode(int g, int h, const iPoint & pos, const PathNode * parent) : g(g), h(h), pos(pos), parent(parent), on_close(false), on_open(false)
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



ToPath::ToPath(Unit * getUnit, const iPoint & destination, const iPoint& target, bool priority, TASK_CHANNELS task) :unit(getUnit), destination(destination), target(target), priority(priority),task(task)
{
}

ToPath::~ToPath()
{
	unit = nullptr;
}

bool ToPath::operator==(const ToPath& unit)const
{
	return *this->unit == *unit.unit;
}

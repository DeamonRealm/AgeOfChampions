#ifndef _PATHFINDING_H_
#define _PATHFINDING_H_

#include "j1Module.h"
#include "p2Point.h"
#include "BaseEntities.h"
#include <queue>
#define INVALID_WALK_CODE 255
#define MAX_PATH_TIME 5

struct PathList;
struct Node;
struct SDL_Texture;
struct PathNode;
struct ToPath
{
	ToPath(Unit* unit, const iPoint& destination,  bool priority = true, TASK_CHANNELS task_channel = PRIMARY,Unit* unit_target=nullptr);
	~ToPath();
	Unit* unit = nullptr;
	Unit* unit_target = nullptr;

	iPoint destination = { -1,-1 };
	bool priority = true;
	bool operator ==(const ToPath& unit)const;
	TASK_CHANNELS task_channel = TASK_CHANNELS::PRIMARY;

};
///class Pathfinding ------------------
class j1Pathfinding : public j1Module
{
public:

	j1Pathfinding();
	~j1Pathfinding();

	void Init();
	void Disable();
	void Reset();

	// Called before the first frame
	bool Start();
	bool PreUpdate();
	// Called before quitting
	bool CleanUp();
	void SetMap(uint width, uint height);
	void SetMapLimits(int position_x, int position_y, int width, int height);
	uchar GetValueMap(int x, int y) const;
	PathNode* GetPathNode(int x, int y);
private:

	PathNode* path_nodes = nullptr;
	int width = 0;
	int height = 0;
	int map_min_x = 0;
	int map_min_y = 0;
	int map_max_x = 0;
	int map_max_y = 0;

	//Map cluster abstraction

public:

	//Path tile debug texture
	std::list<ToPath> to_path;
	j1Timer pathTime;

public:

	//Functionality ---------
	//Methods used during the paths creation to work with map data
	// Check if the cell coordinate is walkable
	bool	IsWalkable(const iPoint& destination)const;
	// Check if the boundaries of x coordinate are walkable
	bool	CheckBoundaries(const iPoint& pos) const;
	// Get tile from x coordinate
	uchar	GetTileAt(const iPoint& pos) const;
	// Create a path with two nodes
	std::vector<iPoint>* SimpleAstar(const iPoint& origin, const iPoint& goal);
	// Create a path with two coordinates
	void PushPath(Unit* unit, iPoint destination, bool priority = true, TASK_CHANNELS task = PRIMARY,Unit* unit_target = nullptr);
	
};
/// -----------------------------------

/// Struct PathNode -------------------
//Helper struct to represent a node in the path creation
struct PathNode
{
	//Constructors ----------
	PathNode();
	PathNode(int g, int h, const iPoint& pos, const PathNode* parent);
	PathNode(const PathNode& node);

	//Methods----------------
	// Fills a list (PathList) of all valid adjacent path nodes
	uint FindWalkableAdjacents(PathList* list_to_fill) const;
	// Calculates this tile score
	float Score() const;
	// Calculate the F for a specific destination tile
	int CalculateF(const iPoint& destination);
	void SetPosition(const iPoint& value);
	//Operators -------------
	bool operator ==(const PathNode& node)const;
	bool operator !=(const PathNode& node)const;

	// PathNode data --------
	float			g = -1;
	int				h = -1;
	iPoint			pos = { -1,-1 };
	bool			on_close = false;
	bool			on_open = false;
	const PathNode* parent = nullptr; // needed to reconstruct the path in the end

};
/// -----------------------------------

///Struct PathList --------------------
//Helper struct to include a list of path nodes
struct PathList
{

	//Methods ---------------
	// Looks for a node in this list and returns it's list node or NULL
	//	std::list<PathNode>::iterator Find(const iPoint& point);
	// Returns the path node with lowest score in this list or NULL if empty
	//PathNode* GetNodeLowestScore() const;

	// PathList data --------
	std::list<PathNode*> list;

};
struct compare
{
	bool operator()(const PathNode* l, const PathNode* r)
	{
		return l->Score() >= r->Score();
	}
};
struct OpenList
{
public:
	//Methods ---------------
	// Looks for a node in this list and returns it's list node or NULL
	//std::list<PathNode>::iterator Find(const iPoint& point);
	// Returns the path node with lowest score in this list or NULL if empty
	//PathNode* GetNodeLowestScore() const;

	// PathList data --------
	std::priority_queue<PathNode*, std::vector<PathNode*>, compare > queue;
};

/// -----------------------------------
#endif // _PATHFINDING_H_

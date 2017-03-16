#ifndef _PATHFINDING_H_
#define _PATHFINDING_H_

#include "j1Module.h"
#include "p2Point.h"
#include <queue>
struct PathList;
struct Node;
struct ClusterAbstraction;
struct SDL_Texture;
struct PathNode;
///class Pathfinding ------------------
class j1Pathfinding : public j1Module
{
public:

	j1Pathfinding();
	~j1Pathfinding();

	// Called before the first frame
	bool Start();

	// Called before quitting
	bool CleanUp();
	void SetMap(uint width, uint height, uchar*& data);
	uchar GetValueMap(int x, int y) const;
	PathNode* GetPathNode(int x, int y);
private:
	uchar* logic_map=nullptr;
	PathNode* path_nodes=nullptr;
	int width=0;
	int height=0;
	//A pointer to the last path generated
	std::vector<iPoint> last_path;
	//Map cluster abstraction
	ClusterAbstraction* cluster_abstraction = nullptr;

public:

	//Path tile debug texture
	SDL_Texture* path_texture = nullptr;

public:

	//Functionality ---------
	//Methods used during the paths creation to work with map data
	// Check if the cell coordinate is walkable
	void	InitClusterAbstraction();
	bool	IsWalkable(const iPoint& destination)const;
	// Check if the boundaries of x coordinate are walkable
	bool	CheckBoundaries(const iPoint& pos) const;
	// Get tile from x coordinate
	uchar	GetTileAt(const iPoint& pos) const;

	// Create a path with two nodes
	int	CreatePath(Node* start, Node*goal);
	std::vector<iPoint>* CreatePath(const iPoint& origin, const iPoint& goal);
	std::vector<iPoint>* SimpleAstar(const iPoint& origin, const iPoint& goal);

	// Create a path with two coordinates

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

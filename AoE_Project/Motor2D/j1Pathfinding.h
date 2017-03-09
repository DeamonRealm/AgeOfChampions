#ifndef _PATHFINDING_H_
#define _PATHFINDING_H_

#include "j1Module.h"
#include "p2Point.h"

struct PathList;
struct Node;

///class Pathfinding ------------------
class j1Pathfinding : public j1Module
{
public:

	j1Pathfinding();
	~j1Pathfinding();

private:

	//Copy of the current map logic data
	uchar*	logic_map = nullptr;
	//Logic map dimensions
	uint	width = 0;
	uint	height = 0;
	//A pointer to the last path generated
	std::vector<iPoint> last_path;

public:

	//Functionality ---------
	//Methods used during the paths creation to work with map data
	// Check if the cell coordinate is walkable
	bool	IsWalkable(const iPoint& destination)const;
	// Check if the boundaries of x coordinate are walkable
	bool	CheckBoundaries(const iPoint& pos) const;
	// Get tile from x coordinate
	uchar	GetTileAt(const iPoint& pos) const;

	// Set copy map data
	void SetMap(uchar* map, int map_width, int map_height);
	// Create a path with two nodes
	int	CreatePath(Node* start, Node*goal);
	// Create a path with two coordinates
	std::vector<iPoint>* CreatePath(const iPoint& origin, const iPoint& goal);

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
	uint FindWalkableAdjacents(PathList& list_to_fill) const;
	// Calculates this tile score
	float Score() const;
	// Calculate the F for a specific destination tile
	int CalculateF(const iPoint& destination);

	//Operators -------------
	bool operator ==(const PathNode& node)const;
	bool operator !=(const PathNode& node)const;

	// PathNode data --------
	float			g = 0;
	int				h = 0;
	iPoint			pos = { 0,0 };
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
	std::list<PathNode>::iterator Find(const iPoint& point);
	// Returns the path node with lowest score in this list or NULL if empty
	PathNode* GetNodeLowestScore() const;

	// PathList data --------
	std::list<PathNode> list;

};
/// -----------------------------------
#endif // _PATHFINDING_H_

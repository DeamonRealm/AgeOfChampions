#ifndef _J1ASTAR_H_
#define _J1ASTAR_H_

#include "j1Map.h"
#include "j1ClusterAbstraction.h"


struct PathList;

///Class Astar ------------------------
//Class to manage a A* pathfinding
class j1Astar
{
public:

	j1Astar();
	~j1Astar();
	
	// Check if the cell coordinate is walkable
	bool	IsWalkable(iPoint& destination)const;
	// Check if the boundaries of x coordinate are walkable
	bool	CheckBoundaries(const iPoint& pos) const;
	// Get tile from x coordinate
	uchar	GetTileAt(const iPoint& pos) const;
	// Set copy map data
	void	SetMap(uchar*logic_map, int width, int height);
	// Create a path with two nodes
	int		CreatePath(Node* start, Node*goal);

private:

	uchar*				copy_map = nullptr;
	int					width = 0;
	int					height = 0;
	std::vector<iPoint> last_path;

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
	// Fills a list (PathList) of all valid adjacent pathnodes
	uint FindWalkableAdjacents(PathList& list_to_fill,j1Astar* Astar) const;
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
	const PathNode* parent; // needed to reconstruct the path in the end

};
/// -----------------------------------

///Struct PathList --------------------
//Helper struct to include a list of path nodes
struct PathList
{

	//Methods ---------------
	// Looks for a node in this list and returns it's list node or NULL
	std::list<PathNode>::iterator Find(const iPoint& point);
	// Returns the Pathnode with lowest score in this list or NULL if empty
	PathNode* GetNodeLowestScore() const;

	// PathList data --------
	std::list<PathNode> list;

};
/// -----------------------------------

#endif // _J1ASTAR_H_

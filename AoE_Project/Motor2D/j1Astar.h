#pragma once
#ifndef _J1ASTAR_H_
#define _J1ASTAR_H_
#include "j1Map.h"
#include "j1ClusterAbstraction.h"
struct PathList;

class j1Astar
{
public:
	j1Astar();
	~j1Astar();
	bool IsWalkable(iPoint& destination)const;
	bool CheckBoundaries(const iPoint& pos) const;
	uchar GetTileAt(const iPoint& pos) const;

	void SetMap(uchar*logic_map, int width, int height);
	int CreatePath(Node* start, Node*goal);
private:
	std::vector<iPoint> last_path;
	int width;
	int height;
	uchar* copy_map;
};
// -------------------------------------------------------------------- -
// Pathnode: Helper struct to represent a node in the path creation
// ---------------------------------------------------------------------
struct PathNode
{
	// Convenient constructors
	PathNode();
	PathNode(int g, int h, const iPoint& pos, const PathNode* parent);
	PathNode(const PathNode& node);

	// Fills a list (PathList) of all valid adjacent pathnodes
	uint FindWalkableAdjacents(PathList& list_to_fill,j1Astar* Astar) const;

	// Calculates this tile score
	float Score() const;
	// Calculate the F for a specific destination tile
	int CalculateF(const iPoint& destination);

	// -----------
	float g;
	int h;
	int id;
	iPoint pos;
	const PathNode* parent; // needed to reconstruct the path in the end
};

// ---------------------------------------------------------------------
// Helper struct to include a list of path nodes
// ---------------------------------------------------------------------
struct PathList
{
	// Looks for a node in this list and returns it's list node or NULL
	std::list<PathNode>::iterator Find(const iPoint& point);

	// Returns the Pathnode with lowest score in this list or NULL if empty
	std::list<PathNode>::const_iterator GetNodeLowestScore() const;

	// -----------
	// The list itself
	std::list<PathNode> list;
};


#endif // !_J1ASTAR_H_

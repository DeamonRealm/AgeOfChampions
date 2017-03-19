#ifndef _QUADTREE_
#define _QUADTREE_

#include <vector>

#define MAX_ELEMENTS_IN_NODE 10
#define NODE_SUBDIVISION 4


#include "SDL/include/SDL_rect.h"
#include "p2Point.h"

/// Class AABB ----------------------------------
// This are the rectangles used by the quad-tree to subdivide the space
class AABB
{
public:

	AABB(const SDL_Rect& aabb);
	~AABB();

public:

	SDL_Rect				aabb = { 0,0,0,0 };
	std::vector<iPoint>		objects;
	bool					full = false;
	AABB*					root = nullptr;
	AABB*					children[NODE_SUBDIVISION];
	
public:

	// Functionality --------
	void	Draw()const;
	bool	Contains(const iPoint* point)const;
	bool	Intersects(const AABB* target)const;
	bool	Insert(iPoint* new_point);
	void	Subdivide();
	int		CollectCandidates(std::vector<iPoint*>& nodes, const SDL_Rect& rect);
	void	CollectPoints(std::vector< AABB* >& nodes);

};
/// ---------------------------------------------

/// Class QuadTree ------------------------------
// This class contains all the tree information & functionality
class QuadTree
{
public:

	QuadTree(const SDL_Rect& rect = { 0,0,0,0 });
	~QuadTree();

private:

	AABB* root = nullptr;

public:

	// Functionality --------
	void	SetBoundaries(const SDL_Rect& r);
	bool	Insert(iPoint* newpoint);
	void	Draw()const;
	int		CollectCandidates(std::vector< iPoint* >& nodes, const SDL_Rect& r) const;
	void	CollectPoints(std::vector< AABB* >& nodes) const;
	void	Clear();

};

/// ---------------------------------------------
#endif // !_QUADTREE_

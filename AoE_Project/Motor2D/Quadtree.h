#ifndef _QUADTREE_
#define _QUADTREE_

#include <vector>

#define NODE_SUBDIVISION 4


#include "SDL/include/SDL_rect.h"
#include "p2Point.h"

/// Class AABB ----------------------------------
// This are the rectangles used by the quad-tree to subdivide the space
class AABB
{
public:

	AABB(const SDL_Rect& aabb, uint max_objects);
	~AABB();

public:

	SDL_Rect				aabb = { 0,0,0,0 };
	std::vector<iPoint>		objects;
	uint					max_objects = 0;
	bool					full = false;
	AABB*					root = nullptr;
	AABB*					children[NODE_SUBDIVISION];
	
public:

	// Functionality --------
	void	Draw(const SDL_Color& color)const;
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

	QuadTree(const SDL_Rect& rect = { 0,0,0,0 }, uint max_objects = 0);
	~QuadTree();

private:

	AABB*		root = nullptr;
	uint		max_objects = 0;
	SDL_Color	color = { 255,255,255,255 };

public:

	// Functionality --------
	void	SetBoundaries(const SDL_Rect& r);
	void	SetMaxObjects(uint max);
	void	SetDebugColor(const SDL_Color& new_color);
	bool	Insert(iPoint* newpoint);
	void	Draw()const;
	int		CollectCandidates(std::vector< iPoint* >& nodes, const SDL_Rect& r) const;
	void	CollectPoints(std::vector< AABB* >& nodes) const;
	void	Clear();

};

/// ---------------------------------------------
#endif // !_QUADTREE_

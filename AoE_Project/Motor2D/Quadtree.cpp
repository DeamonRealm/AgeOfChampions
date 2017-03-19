#include "Quadtree.h"

#include "j1App.h"
#include "j1Render.h"

/// Class AABB ----------------------------------
//Constructors ========================
AABB::AABB(const SDL_Rect & aabb) :aabb(aabb)
{
	for (uint i = 0; i < 4; i++)
		children[i] = nullptr;
}

//Destructors =========================
AABB::~AABB()
{
	for (int i = 0; i < 4; ++i)
		if (children[i] != nullptr)
			delete children[i];
}

void AABB::Draw() const
{
	App->render->DrawQuad(aabb, 255, 255, 255, 255, false);
	for (uint k = 0; k < NODE_SUBDIVISION; k++)
	{
		if (children[k] != nullptr)
		{
			children[k]->Draw();
		}
	}
}

//Functionality =======================
bool AABB::Contains(const iPoint* point) const
{
	SDL_Point p = { point->x,point->y };
	return SDL_PointInRect(&p, &aabb);
}

bool AABB::Intersects(const AABB* target) const
{
	return SDL_HasIntersection(&aabb, &target->aabb);
}

bool AABB::Insert(iPoint * new_point)
{
	// If new point is not in the quad-tree AABB, return
	SDL_Point p = { new_point->x,new_point->y };
	if (!SDL_PointInRect(&p, &this->aabb))
	{
		return false;
	}

	if (full)
	{
		for (uint i = 0; i < NODE_SUBDIVISION; i++)
		{
			if (children[i]->Insert(new_point))
			{
				return true;
			}
		}
		return false;
	}

	// If in this node there is space for the point, pushback it
	uint size = objects.size();
	if (size < MAX_ELEMENTS_IN_NODE)
	{
		objects.push_back(*new_point);
		if (size + 1 == MAX_ELEMENTS_IN_NODE)
		{
			full = true;
			Subdivide();
		}
		return true;
	}

	return false;
}

void AABB::Subdivide()
{
	// Get subdivision size
	iPoint qSize = { (int)floor(aabb.w * 0.5), (int)floor(aabb.h * 0.5) };
	iPoint qCentre;

	//Calculate new AABB center for each child
	qCentre = { aabb.x,aabb.y };
	children[0] = new AABB({ qCentre.x,qCentre.y,qSize.x,qSize.y });
	children[0]->root = this;


	qCentre = { aabb.x + qSize.x,aabb.y };
	children[1] = new AABB({ qCentre.x,qCentre.y,qSize.x,qSize.y });
	children[1]->root = this;

	qCentre = { aabb.x,aabb.y + qSize.y };
	children[2] = new AABB({ qCentre.x,qCentre.y,qSize.x,qSize.y });
	children[2]->root = this;
	
	qCentre = { aabb.x + qSize.x,aabb.y + qSize.y };
	children[3] = new AABB({ qCentre.x,qCentre.y,qSize.x,qSize.y });
	children[3]->root = this;

	uint size = objects.size();
	for (uint h = 0; h < size; h++)
	{
		for (uint k = 0; k < NODE_SUBDIVISION; k++)
		{
			if (children[k]->Insert(&objects[h])) break;
		}
	}

}

int AABB::CollectCandidates(std::vector<iPoint*>& nodes, const SDL_Rect & rect)
{
	uint ret = 0;

	// If range is not in the quad-tree, return
	if (!SDL_HasIntersection(&rect, &aabb)) return 0;

	// See if the points of this node are in range and pushback them to the vector
	if (children[0] != nullptr)
	{
		// Otherwise, add the points from the children
		ret += children[0]->CollectCandidates(nodes, rect);
		ret += children[1]->CollectCandidates(nodes, rect);
		ret += children[2]->CollectCandidates(nodes, rect);
		ret += children[3]->CollectCandidates(nodes, rect);
	}
	else if (!objects.empty())
	{
		uint size = objects.size();
		for (uint k = 0; k < size; k++)
		{
			if (SDL_PointInRect((SDL_Point*)&objects[k], &rect))
				nodes.push_back(&objects[k]);
		}
	}

	return ret;
}

void AABB::CollectPoints(std::vector<AABB*>& nodes)
{
	nodes.push_back(this);
	for (int i = 0; i < 4; ++i)
		if (children[i] != NULL) children[i]->CollectPoints(nodes);
}
/// ---------------------------------------------


/// Class QuadTree ---------------------------
//Constructors ========================
QuadTree::QuadTree(const SDL_Rect & rect)
{
	SetBoundaries(rect);
}

//Destructors =========================
QuadTree::~QuadTree()
{
	Clear();
}

//Functionality =======================
void QuadTree::SetBoundaries(const SDL_Rect & r)
{
	if (root != NULL)
		delete root;

	root = new AABB(r);
}

bool QuadTree::Insert(iPoint* newpoint)
{
	if (root != NULL)
	{
		return root->Insert(newpoint);
	}
	return false;
}

void QuadTree::Draw() const
{
	root->Draw();
}

int QuadTree::CollectCandidates(std::vector<iPoint*>& nodes, const SDL_Rect & r) const
{
	int tests = 1;
	
	if (root != NULL && SDL_HasIntersection(&r, &root->aabb))
		tests = root->CollectCandidates(nodes, r);
	return tests;
}

void QuadTree::CollectPoints(std::vector<AABB*>& nodes) const
{
	if (root != NULL)
		root->CollectPoints(nodes);
}

void QuadTree::Clear()
{
	if (root != NULL)
	{
		delete root;
		root = NULL;
	}
}

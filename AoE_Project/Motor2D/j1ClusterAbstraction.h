#pragma once
#ifndef _J1CLUSTER_ABSTRACTION_H_
#define _J1CLUSTER_ABSTRACTION_H_
#include <vector>
#include "j1Map.h"
#include "j1LogicMap.h"
enum Orientation {
	HORIZONTAL,
	VERTICAL
};
#define MAX_ENTRY_NUM 6
#define INVALID_WALK_CODE 255
class Cluster
{
public:

	Cluster(int posX,int posY,int width,int height,int row,int column,int id);
	~Cluster();

private:

	int						posX;
	int						posY;
	int						width;
	int						height;
	int						row;
	int						column;
	int						id;
	std::vector<int>		nodes;
};

class Entry
{
public:

	/*Entry(int row, int column,Orientation orientation);
	~Entry();*/

	int						row;
	int						column;
	Orientation				orientatiton;

private:

};

class j1ClusterAbstraction
{
public:
	j1ClusterAbstraction(j1Map* map, uint clusterSize);
	~j1ClusterAbstraction();
	void CreateClusters();
	void SetMap(uint width, uint height, uchar* data);
	void AddCluster(Cluster add);
	bool IsWalkable(int x, int y) const;
	bool CheckBoundaries(const iPoint& pos) const;
	uchar GetTileAt(const iPoint& pos) const;

	void CreateEntryHorizontal(int start, int end, int x, int row, int column);
	void CreateEntryVertical(int start, int end, int x, int row, int column);

private:
	// size of the map 
	uint					width;
	uint					height;
	uint					clusterSize;
	std::vector<Entry>		entrys;
	std::vector<Cluster>	clusters;
	//This only works with a single level of terrain in case we have more than one unit size we will have to add a vector map
	uchar* map;

};




#endif // !_J1CLUSTER_H_

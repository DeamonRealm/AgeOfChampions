#pragma once
#ifndef _J1CLUSTER_ABSTRACTION_H_
#define _J1CLUSTER_ABSTRACTION_H_
#include <vector>
#include "j1Map.h"
enum Orientation {
	HORIZONTAL,
	VERTICAL
};
class Cluster
{
public:
	Cluster(int posX,int posY,int width,int height,int row,int column,int id);
	~Cluster();

private:
	int posX;
	int posY;
	int width;
	int height;
	int row;
	int column;
	int id;
	std::vector<int> nodes;
};

class Entry
{
public:
	Entry();
	~Entry();

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
private:
	// size of the map 
	uint width;
	uint height;
	uint clusterSize;

	std::vector<Cluster> clusters;
	//This only works with a single level of terrain in case we have more than one unit size we will have to add a vector map
	uchar* map;

};




#endif // !_J1CLUSTER_H_

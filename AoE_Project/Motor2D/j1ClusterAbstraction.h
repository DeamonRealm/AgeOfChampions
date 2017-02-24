#pragma once
#ifndef _J1CLUSTER_ABSTRACTION_H_
#define _J1CLUSTER_ABSTRACTION_H_
#include <vector>
#include "j1Map.h"
#include "j1LogicMap.h"
enum ClusterOrientation {
	CLUSTER_HORIZONTAL,
	CLUSTER_VERTICAL
};
#define MAX_ENTRY_NUM 6
#define INVALID_WALK_CODE 255

class Cluster;

class Edge
{
	Edge(int nodeNum1,int nodeNum2,int distance);
	~Edge();
public:
private:
	int distance;
	int nodeNum1;
	int nodeNum2;
};
struct Node
{
public:
	int nodeNum;

private:
	Cluster* clusterID;
	int posX;
	int posY;
};

class Graph
{
public:
	/*
	Graph();
	~Graph();
	void AddEdge(Edge* edge);
	*/
	int AddNode(Node* edge);

private:
	std::vector<Node*> nodes;
	std::vector<Edge*> edges;
};
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

	Entry(int x,int y, int clusterID1,int clusterID2,int row, int column,int lenght, ClusterOrientation orientation);
	~Entry();
	int GetPosX();
	int GetPosY();
	int GetRow();
	int GetColumn();
	ClusterOrientation GetOrientation();

	void SetClusterID1(int clusterID1);
	void SetClusterID2(int clusterID2);

private:
	int						posX;
	int						posY;
	int						clusterID1;
	int						clusterID2;
	int						lenght;
	int						row;
	int						column;
	ClusterOrientation		orientation;

private:

};

class j1ClusterAbstraction
{
public:
	j1ClusterAbstraction(j1Map* map, uint clusterSize);
	~j1ClusterAbstraction();
	//Map extraction
	void SetMap(uint width, uint height, uchar* data);
	bool IsWalkable(int x, int y) const;
	bool CheckBoundaries(const iPoint& pos) const;
	uchar GetTileAt(const iPoint& pos) const;
	//Cluster Functions
	void CreateClusters();
	void AddCluster(Cluster add);
	Cluster& GetCluster(int at);
	int  GetClusterID(int clusterRow, int clusterColumn);
	//
	void CreateEntryHorizontal(int start, int end, int x, int row, int column);
	void CreateEntryVertical(int start, int end, int x, int row, int column);

	void SetEntryClusterID();
	void CreateGraph();
	void SetNodesOnClusters(Graph* graph);

private:
	// size of the map 
	uint					width;
	uint					height;
	uint					clusterSize;
	std::vector<Entry>		entrys;
	std::vector<Cluster>	clusters;
	//This only works with a single level of terrain in case we have more than one unit size we will have to add a vector map
	uchar* map;
	int maxColumn;
	int maxRow;
};




#endif // !_J1CLUSTER_H_

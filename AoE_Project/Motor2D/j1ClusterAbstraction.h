#pragma once
#ifndef _J1CLUSTER_ABSTRACTION_H_
#define _J1CLUSTER_ABSTRACTION_H_
#include <list>
#include <vector>
#include "j1Map.h"
#include "j1LogicMap.h"
enum ClusterOrientation {
	CLUSTER_HORIZONTAL,
	CLUSTER_VERTICAL
};
enum EdgeType
{
INTER_EDGE,
INTRA_EDGE

};
#define MAX_ENTRY_NUM 6
#define INVALID_WALK_CODE 255

class Cluster;

class Edge
{
public:

	Edge(int nodeNum1,int nodeNum2,int distance,EdgeType type);
	~Edge();
	int GetNodeNum1();
	int GetNodeNum2();
private:
	int distance;
	int nodeNum1;
	int nodeNum2;
	EdgeType type;
};
struct Node
{
public:
	void SetPosition(int posX, int posY);
	void SetClusterID(int id);

	int GetPositionX();
	int GetPositionY();
	int nodeNum;
	
private:
	int clusterID;
	int posX;
	int posY;
};

class Graph
{
public:
	/*
	Graph();
	~Graph();
	*/
	Node* GetNode(int i);
	void AddEdge(Edge* edge);
	
	int AddNode(Node* edge);
	int EdgeSize();
	Edge* EdgeAt(int i);
private:
	std::vector<Node*> nodes;
	std::vector<Edge*> edges;
};
class Cluster
{
public:

	Cluster(int posX,int posY,int width,int height,int row,int column,int id);
	~Cluster();
	int NodeSize();
	int GetNodeNumberAt(int i);
	int GetPosisitionX();
	int GetPosisitionY();
	int GetWidth();
	int GetHeight();
	void AddNode(int get);
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
	int GetClusterID1();
	int GetClusterID2();
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
	uchar GetValueMap(int x, int y);
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

	int NodeExist(Cluster& cluster,int posX,int posY, Graph* graph);
	bool EdgeExist(Cluster& cluster, int nodeID1, int nodeID2, Graph* graph);

	void SetEntryClusterID();
	void CreateGraph();
	void SetNodesOnClusters(Graph* graph);
	void CreateIntraEdges(Graph* graph);

private:
	// size of the map 
	uint					width;
	uint					height;
	uint					clusterSize;
	Graph					graph;
	std::vector<Entry>		entrys;
	std::vector<Cluster>	clusters;
	//This only works with a single level of terrain in case we have more than one unit size we will have to add a vector map
	uchar* map;
	int maxColumn;
	int maxRow;
};




#endif // !_J1CLUSTER_H_

#ifndef _J1CLUSTER_ABSTRACTION_H_
#define _J1CLUSTER_ABSTRACTION_H_
#include <list>
#include <vector>
#include <queue>
#include "j1Map.h"

enum ClusterOrientation
{
	NO_CLUSTER,
	CLUSTER_HORIZONTAL,
	CLUSTER_VERTICAL
	
};
enum EdgeType
{
NO_EDGE,
INTER_EDGE,
INTRA_EDGE
};
#define MAX_ENTRY_NUM 6
#define INVALID_WALK_CODE 255

class Cluster;
struct Node;
class Edge
{
public:

	Edge(Node* nodeNum1, Node* nodeNum2,int distance,EdgeType type);
	~Edge();
	Node* GetNodeNum1();
	Node* GetNodeNum2();
	Node* GetTheOtherNode(int nodeID);
	int GetDistance();
	EdgeType GetEdgeType();
	Edge* RemoveParentNode();
private:
	int distance = 0;
	Node* nodeNum1 = nullptr;
	Node* nodeNum2 = nullptr;
	EdgeType type = NO_EDGE;
};
struct Node
{
public:
	void SetPosition(int posX, int posY);
	void SetClusterID(int id);
	void SetParent(Edge* edge);
	void SetTrack(Node* get);

	int GetPositionX();
	int GetPositionY();
	Edge* GetParentIDAt(int index);
	int GetParentSize();
	Node* GetTrackBack();
	int GetClusterId();
	void RemoveParent(Edge* edge);
	void ResetNode();

	int nodeNum = 0;
	bool visited = false;

private:
	int clusterID = 0;
	int posX = 0;
	int posY = 0;
	std::list<Edge*> parent;
	Node* track_back = nullptr;

};

class Graph
{
public:
	/*
	Graph();
	~Graph();
	*/
	Node* GetNode(int i);
	Edge* AddEdge(Edge* edge);
	
	int AddNode(Node* edge);
	void RemoveNode(Node* node);
	void RemoveEdge(Edge* edge);
	int EdgeSize();
	int GetNodesSize()const;
	Node* GetNodesAt(int index)const;

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
	int GetClusterId();
	void SetClusterMap(uchar* data);
	void AddNode(int get);
private:

	int						posX = 0;
	int						posY = 0;
	int						width = 0;
	int						height = 0;
	int						row = 0;
	int						column = 0;
	int						id=0;
	std::vector<int>		nodes;
	uchar*					logic_cluster_map = nullptr;

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
	int						posX=0;
	int						posY=0;
	int						clusterID1=0;
	int						clusterID2=0;
	int						lenght=0;
	int						row=0;
	int						column=0;
	ClusterOrientation		orientation = NO_CLUSTER;

private:

};

class ClusterAbstraction
{
public:
	ClusterAbstraction(j1Map* map, uint clusterSize);
	~ClusterAbstraction();

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
	int GetClusterID(int clusterRow, int clusterColumn);
	Cluster& FindClusterID(iPoint position);
	//
	void CreateEntryHorizontal(int start, int end, int x, int row, int column);
	void CreateEntryVertical(int start, int end, int x, int row, int column);
	//Node Functions
	int NodeExist(Cluster& cluster,int posX,int posY, Graph* graph);
	Node* PutNode(const iPoint& pos);
	
	void DeleteNode(Node* start, Node* goal);
	void DeleteNode(Node* node);
	int GetDistanceOver(Node* start, Node* goal);
	// Edge Functions
	bool EdgeExist(Cluster& cluster, int nodeID1, int nodeID2, Graph* graph);

	void SetEntryClusterID();
	void CreateGraph();
	void SetNodesOnClusters(Graph* graph);
	void CreateIntraEdges(Graph* graph);

	int CreateBFS(Node* from, Node* to);
	std::vector<Node*> GetBestPath();
	Node* GetBestPathAt(int index);
	Graph					graph;

public:

	// size of the map 
	uint					width = 0;
	uint					height = 0;
	uint					clusterSize = 0;
	std::vector<Entry>		entrys;
	std::vector<Cluster>	clusters;
	std::vector<Node*>		best_path;


	//This only works with a single level of terrain in case we have more than one unit size we will have to add a vector map
	uchar* logic_map = nullptr;
	int maxColumn = 0;
	int maxRow = 0;

};




#endif // !_J1CLUSTER_H_

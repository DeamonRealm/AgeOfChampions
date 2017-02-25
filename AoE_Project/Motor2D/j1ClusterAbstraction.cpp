#include "j1ClusterAbstraction.h"
#include "p2Log.h"

Cluster::Cluster(int posX, int posY, int width, int height, int row, int column, int id):posX(posX),posY(posY),width(width), height(height),row(row),column(column),id(id)
{
}

Cluster::~Cluster()
{
}
int Cluster::NodeSize() {
	return nodes.size();
}
int Cluster::GetNodeNumberAt(int i) {
	return nodes[i];
}
int Cluster::GetPosisitionX() {
	return posX;
}
int Cluster::GetPosisitionY() {
	return posY;

}
void Cluster::AddNode(int get) 
{
	nodes.push_back(get);
}
j1ClusterAbstraction::j1ClusterAbstraction(j1Map * m, uint clusterSize):clusterSize(clusterSize)
{
	if (m->CreateWalkabilityMap(width, height, &map))
		SetMap(width, height, map);
	CreateClusters();
	SetEntryClusterID();
	SetNodesOnClusters(&graph);
}

j1ClusterAbstraction::~j1ClusterAbstraction()
{
	RELEASE_ARRAY(map);
}
void j1ClusterAbstraction::CreateClusters()
{
	int row = 0, column = 0, clusterID = 0;
	int clusterW = 0, clusterH = 0;
	for (int i = 0; i < height; i += clusterSize) {
		column = 0;
		for (int j = 0; j < width; j += clusterSize) {
			clusterW = MIN(clusterSize, width - j);
			clusterH = MIN(clusterSize, height - i);
			Cluster cluster(j, i, clusterW, clusterH, row, column, clusterID++);
			AddCluster(cluster);
			if (i > 0 && i < width) {
				// (i = 0 clusterH-1 = 9 )== 10
				//
				CreateEntryHorizontal(j, j + clusterW -1, i - 1, row - 1, column);
			}
			if (j > 0 && j < height) {
				CreateEntryVertical(i, i + clusterH -1, j - 1, row, column - 1);
			}

			column++;
		}
		row++;
	}
	maxColumn = column;
	maxRow = row;
}
void j1ClusterAbstraction::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = new uchar[width*height];
	memcpy(map, data, width*height);
}

void j1ClusterAbstraction::AddCluster(Cluster add)
{
	clusters.push_back(add);
}

Cluster & j1ClusterAbstraction::GetCluster(int at)
{
	if (at >= 0 && at < (int)clusters.size()) {
		return clusters[at];
	}
}

int j1ClusterAbstraction::GetClusterID(int clusterRow, int clusterColumn)
{

	return (clusterRow*maxColumn+clusterColumn);
}

bool j1ClusterAbstraction::IsWalkable(int x, int y) const
{
		bool ret = false;
		iPoint pos(x, y);
		uchar t = GetTileAt(pos);
		if (t != INVALID_WALK_CODE && t > 0) {
			
				ret = true;
			
		}
		return ret;
	}

// Utility: return true if pos is inside the map boundaries
bool j1ClusterAbstraction::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
		pos.y >= 0 && pos.y <= (int)height);
}


// Utility: return the walkability value of a tile
uchar j1ClusterAbstraction::GetTileAt(const iPoint& pos) const
{
	if (CheckBoundaries(pos))
		return map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}

void j1ClusterAbstraction::CreateEntryHorizontal(int start, int end, int y, int row, int column)
{
	int begin = 0;
	for (int i = start; i <= end; i++) {
		while ((i <= end) && !IsWalkable(i,y) && !IsWalkable(i,y+1))
		{
			i++;
		}

		if (i > end)
			return;

		begin = i;
		i++;
		while ((i <= end) && IsWalkable(i, y) && IsWalkable(i, y + 1))
		{
			i++;
		}
		if ((i - begin) >= MAX_ENTRY_NUM){
			Entry entry1(begin, y, -1, -1, row, column,1, CLUSTER_HORIZONTAL);
			entrys.push_back(entry1);
			Entry entry2 (i-1, y, -1, -1, row, column,1, CLUSTER_HORIZONTAL);
			entrys.push_back(entry2);

		}
		else {
			Entry entry(((i-1)+begin)*0.5, y, -1, -1, row, column, 1, CLUSTER_HORIZONTAL);
			entrys.push_back(entry);
		}
		i--;
	}
}

void j1ClusterAbstraction::CreateEntryVertical(int start, int end, int x, int row, int column)
{
	int begin = 0;
	for (int i = start; i <= end; i++) {
		while ((i <= end) && !IsWalkable(x,i) && !IsWalkable(x+1,i))
		{
			i++;
		}
		if (i > end)
			return;

		begin = i;
		i++;
		while ((i <= end) && IsWalkable(x, i) && IsWalkable(x + 1, i))
		{
			i++;
		}
		if ((i - begin) >= MAX_ENTRY_NUM) {
			Entry entry1(x,begin, -1, -1, row, column, 1, CLUSTER_VERTICAL);
			entrys.push_back(entry1);
			Entry entry2(x,i - 1, -1, -1, row, column, 1, CLUSTER_VERTICAL);
			entrys.push_back(entry2);

		}
		else {
			Entry entry(x,((i - 1) + begin)*0.5, -1, -1, row, column, 1, CLUSTER_VERTICAL);
			entrys.push_back(entry);
		}
		i--;
	}
}
int j1ClusterAbstraction::NodeExist(Cluster& cluster, int posX, int posY, Graph* graph) {
	int ret = -1;
	for (int i = 0; i < cluster.NodeSize(); i++) {
		ret = cluster.GetNodeNumberAt(i);
		Node* node = graph->GetNode(ret);
		if (node->GetPositionX() == posX && node->GetPositionY() == posY) {
			return ret;
		}
	}
	return -1;

}
void j1ClusterAbstraction::SetEntryClusterID()
{

	int clusterID1;
	int clusterID2;
	for (int i = 0; i < entrys.size(); i++) {
		Entry &item = entrys[i];
		switch (item.GetOrientation())
		{
		case CLUSTER_HORIZONTAL:
			//Get Top
			clusterID1 = GetClusterID(item.GetRow(), item.GetColumn());
			item.SetClusterID1(clusterID1);
			//Get Bottom
			clusterID2 = GetClusterID(item.GetRow()+1, item.GetColumn());
			item.SetClusterID2(clusterID2);
			break;
		case CLUSTER_VERTICAL:
			//Get Left
			clusterID1 = GetClusterID(item.GetRow(), item.GetColumn());
			item.SetClusterID1(clusterID1);
			//Get Right
			clusterID2 = GetClusterID(item.GetRow(), item.GetColumn()+1);
			item.SetClusterID2(clusterID2);
			break;
		default:
			break;
		}
		

	}
}

void j1ClusterAbstraction::CreateGraph()
{
	SetNodesOnClusters(&graph);
}

void j1ClusterAbstraction::SetNodesOnClusters(Graph* graph)
{
	int checkNum = -1;
	int numNode1 = -1;
	int numNode2 = -1;
	int clusterID1 = 0;
	int clusterID2 = 0;
	for (int i = 0; i < entrys.size(); i++) {
		Entry& item = entrys[i];
		
		clusterID1 = item.GetClusterID1();
		clusterID2 = item.GetClusterID2();
		switch (item.GetOrientation())
		{
		case CLUSTER_HORIZONTAL:
		{
			Cluster& cluster1 = GetCluster(clusterID1);
			checkNum = NodeExist(cluster1, cluster1.GetPosisitionX(), cluster1.GetPosisitionY(), graph);
			if (checkNum == -1) {
				Node* node1 = new Node();
				numNode1 = graph->AddNode(node1);
				node1->SetPosition(cluster1.GetPosisitionX(), cluster1.GetPosisitionY());
				node1->SetClusterID(clusterID1);
				cluster1.AddNode(numNode1);
				LOG("Node %i Generated x = %i, y = %i", numNode1,node1->GetPositionX(), node1->GetPositionY() );

			}
			else {
				numNode1 = checkNum;
			}

			Cluster& cluster2 = GetCluster(clusterID2);
			checkNum = NodeExist(cluster2, cluster2.GetPosisitionX(), cluster2.GetPosisitionY()+1, graph);
			if (checkNum == -1) {
				Node* node2 = new Node();
				node2->SetPosition(cluster2.GetPosisitionX(), cluster2.GetPosisitionY() + 1);
				numNode2 = graph->AddNode(node2);
				node2->SetClusterID(clusterID2);
				cluster2.AddNode(numNode2);
				LOG("Node %i Generated x = %i, y = %i", numNode2, node2->GetPositionX(), node2->GetPositionY());

			}
			else {
				numNode2 = checkNum;
			}

			graph->AddEdge(new Edge(numNode1, numNode2, 1));
			LOG("Inter Edge Generated on nodeOne = %i nodeTwo = %i", numNode1, numNode2);
		}

		break;
		case CLUSTER_VERTICAL:
		{
			Cluster& cluster1 = GetCluster(clusterID1);
			checkNum = NodeExist(cluster1, cluster1.GetPosisitionX(), cluster1.GetPosisitionY(), graph);

			if (checkNum == -1) {
				Node* node1 = new Node();
				numNode1 = graph->AddNode(node1);
				node1->SetPosition(cluster1.GetPosisitionX(), cluster1.GetPosisitionY());
				node1->SetClusterID(clusterID1);
				cluster1.AddNode(numNode1);
				LOG("Node %i Generated x = %i, y = %i", numNode1, node1->GetPositionX(), node1->GetPositionY());

			}
			else {
				numNode1 = checkNum;
			}

			Cluster& cluster2 = GetCluster(clusterID2);
			checkNum = NodeExist(cluster2, cluster2.GetPosisitionX()+1, cluster2.GetPosisitionY(), graph);
			if (checkNum == -1) {
				Node* node2 = new Node();
				node2->SetPosition(cluster2.GetPosisitionX() + 1, cluster2.GetPosisitionY());
				numNode2 = graph->AddNode(node2);
				node2->SetClusterID(clusterID2);
				cluster2.AddNode(numNode2);
				LOG("Node %i Generated x = %i, y = %i", numNode2,node2->GetPositionX(), node2->GetPositionY());

			}
			else {
				numNode2 = checkNum;
			}

			graph->AddEdge(new Edge(numNode1, numNode2, 1));
			LOG("Inter Edge Generated on nodeOne = %i nodeTwo = %i", numNode1, numNode2);
		}
		break;
		default:
			break;
		}
	}
}
Node* Graph::GetNode(int i) {
	if (i >= 0 && i < nodes.size()) {
		return nodes[i];
	}
	return 0;
}
void Graph::AddEdge(Edge* edge) {
	if (edge) {
		edges.push_back(edge);
	}
}


int Graph::AddNode(Node * node)
{
	if (node) {
		
		nodes.push_back(node);
		//pushback first because we can't get the size if the size is 0
		node->nodeNum = nodes.size()-1;
		return node->nodeNum;
	}
	//error
	return -1;
}

Edge::Edge(int nodeNum1, int nodeNum2, int distance) : nodeNum1(nodeNum1),nodeNum2(nodeNum2),distance(distance)
{

}
Edge::~Edge()
{
}

void Node::SetPosition(int x, int y) {
	posX = x;
	posY = y;
}
void Node::SetClusterID(int id) {
	clusterID = id;
}
int  Node::GetPositionX() {
	return posX;
}
int  Node::GetPositionY() {
	return posY;

}

Entry::Entry(int posX, int posY, int clusterID1, int clusterID2, int row, int column, int lenght, ClusterOrientation orientation): posX(posX),posY(posY),clusterID1(clusterID1),clusterID2(clusterID2),row(row),column(column),lenght(lenght),orientation(orientation)
{
}

Entry::~Entry()
{
}
int Entry::GetPosX()
{
	return posX;
}

int Entry::GetPosY()
{
	return posY;
}

int Entry::GetRow()
{
	return row;
}

int Entry::GetColumn()
{
	return column;
}
int Entry::GetClusterID1()
{
	return clusterID1;
}int Entry::GetClusterID2()
{
	return clusterID2;
}
ClusterOrientation Entry::GetOrientation()
{
	return orientation;
}

void Entry::SetClusterID1(int clustID1)
{
	clusterID1 = clustID1;
}

void Entry::SetClusterID2(int clustID2)
{
	clusterID2 = clustID2;
}

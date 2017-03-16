#include "ClusterAbstraction.h"

#include "j1App.h"
#include "p2Log.h"
#include "j1Timer.h"
#include "j1Pathfinding.h"

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
int Cluster::GetWidth()
{
	return width;
}
int Cluster::GetHeight()
{
	return height;
}
int Cluster::GetClusterId()
{
	return id;
}
void Cluster::SetClusterMap(uchar* data)
{
	RELEASE_ARRAY(logic_cluster_map);
	logic_cluster_map = new uchar[width*height];
	memcpy(logic_cluster_map, data, width*height);
}
void Cluster::AddNode(int get) 
{
	nodes.push_back(get);
}
ClusterAbstraction::ClusterAbstraction(j1Map * m, uint clusterSize):clusterSize(clusterSize)
{
	j1Timer ptimer;
	uchar* data = nullptr;
	if (m->CreateWalkabilityMap(width, height, &data))
		SetMap(width, height, data);
	LOG("SetMap %f", ptimer.ReadSec());
	ptimer.Start();
	CreateClusters();
	LOG("CreateClusters %f", ptimer.ReadSec());
	ptimer.Start();
	SetEntryClusterID();
	LOG("SetEntryClusterID %f", ptimer.ReadSec());
	ptimer.Start();
	SetNodesOnClusters(&graph);
	LOG("SetNodesOnClusters %f", ptimer.ReadSec());
	ptimer.Start();
	CreateIntraEdges(&graph);
	LOG("CreateIntraEdges %f", ptimer.ReadSec());

}

ClusterAbstraction::~ClusterAbstraction()
{
	RELEASE_ARRAY(logic_map);
	clusters.clear();
	best_path.clear();
	entrys.clear();
}

void ClusterAbstraction::CreateClusters()
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
uchar ClusterAbstraction::GetValueMap(int x, int y)
{
	return logic_map[(y*width)+x];
}
void ClusterAbstraction::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(logic_map);
	logic_map = new uchar[width*height];
	memcpy(logic_map, data, width*height);
}

void ClusterAbstraction::AddCluster(Cluster add)
{
	clusters.push_back(add);
}

Cluster & ClusterAbstraction::GetCluster(int at)
{
	if (at >= 0 && at < (int)clusters.size()) {
		return clusters[at];
	}
}

int ClusterAbstraction::GetClusterID(int clusterRow, int clusterColumn)
{

	return (clusterRow*maxColumn+clusterColumn);
}

Cluster& ClusterAbstraction::FindClusterID(iPoint position)
{
	if (position.x>0 || position.x<width || position.y > 0 || position.y <height) {
		for (int i = 0; i < clusters.size(); i++)
		{
			Cluster& item = clusters[i];
			int posX = item.GetPosisitionX();
			int posY = item.GetPosisitionY();

			int width = item.GetWidth();
			int height = item.GetHeight();
			if (posX <= position.x && posX + width > position.x
				&&posY <= position.y && posY + height > position.y) {
				return item;
			}

		}
	}
}

bool ClusterAbstraction::IsWalkable(int x, int y) const
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
bool ClusterAbstraction::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
		pos.y >= 0 && pos.y <= (int)height);
}


// Utility: return the walkability value of a tile
uchar ClusterAbstraction::GetTileAt(const iPoint& pos) const
{
	if (CheckBoundaries(pos))
		return logic_map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}

void ClusterAbstraction::CreateEntryHorizontal(int start, int end, int y, int row, int column)
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

void ClusterAbstraction::CreateEntryVertical(int start, int end, int x, int row, int column)
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
int ClusterAbstraction::NodeExist(Cluster& cluster, int posX, int posY, Graph* graph) {
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
Node * ClusterAbstraction::PutNode(const iPoint& pos)
{
	Cluster& cluster= FindClusterID(pos);

	int check_num = NodeExist(cluster, pos.x, pos.y, &graph);
	int node_num = 0;
	if (check_num == -1) {
		Node* node = new Node();
		node->SetPosition(pos.x , pos.y);
		node_num = graph.AddNode(node);
		node->SetClusterID(cluster.GetClusterId());
		cluster.AddNode(node_num);
		//	LOG("Node %i Generated x = %i, y = %i", numNode2,node2->GetPositionX(), node2->GetPositionY());
		
		std::vector<Node*> temp_nodes;
		int map_size = cluster.GetHeight()*cluster.GetWidth();
		uchar* temp_map = new uchar[map_size];

		int x = 0;
		int y = 0;
		int node_number = 0;

		int cluster_pos_y = cluster.GetPosisitionY(), cluster_pos_x = cluster.GetPosisitionX();
		int cluster_height = cluster.GetHeight(), cluster_width = cluster.GetWidth();

		Node* conector_node = nullptr;
		for (int i = cluster_pos_y; i < (cluster_pos_y + cluster_height); i++)
		{
			x = 0;
			for (int j = cluster_pos_x; j < (cluster_pos_x + cluster_width); j++)
			{
				temp_map[(y*cluster_width + x)] = GetValueMap(j, i);
				node_number = NodeExist(cluster, j, i, &graph);
				if (node_number != -1) {
					Node* tmp_node = new Node();
					tmp_node->SetPosition(x, y);
					tmp_node->nodeNum = node_number;
					temp_nodes.push_back(tmp_node);
					if (tmp_node->nodeNum == node_num) {
						conector_node = tmp_node;
					}
				}
				x++;
			}
			y++;
		}
		App->pathfinding->SetMap(cluster_width, cluster_height, temp_map);

		int distance = -1;
		int node_size = temp_nodes.size();
		
		for (int i = 0; i < node_size; i++)
		{
			int node_num1 = node_num;
			int node_num2 = temp_nodes[i]->nodeNum;
			if (node_num1 == node_num2)
				continue;

			if (!EdgeExist(cluster, node_num1, node_num2, &graph))
			{
				distance = App->pathfinding->CreatePath(conector_node, temp_nodes[i]);
				if (distance != -1)
				{
					Edge* set = graph.AddEdge(new Edge(graph.GetNode(node_num1), graph.GetNode(node_num2), distance, INTRA_EDGE));
					graph.GetNode(node_num1)->SetParent(set);
					graph.GetNode(node_num2)->SetParent(set);
					LOG("Intra Edge Generated on nodeOne = %i nodeTwo = %i", node_num1, node_num2);
				}
			}
		}
		return node;
	}
	else {
	
		return graph.GetNode(check_num);
	}
}
void ClusterAbstraction::DeleteNode(Node* start, Node* goal)
{

	bool delete_goal = true;
	//Check if goal is a entry node
	for (int i = 0; i < goal->GetParentSize(); i++)
	{
		if (goal->GetParentIDAt(i)->GetEdgeType() == INTRA_EDGE) {
			delete_goal = false;
		}
	}
	if (delete_goal) {
		DeleteNode(goal);
	}
	bool delete_start = true;
	//Check if start is a entry node
	for (int i = 0; i < start->GetParentSize(); i++)
	{
		if (start->GetParentIDAt(i)->GetEdgeType() == INTRA_EDGE) {
			delete_start = false;
		}
	}
	if (delete_start) {
		DeleteNode(start);
	}
}
void ClusterAbstraction::DeleteNode(Node * node)
{
	for (int i = 0; i < node->GetParentSize(); i++)
	{
		Edge* edge = node->GetParentIDAt(i)->RemoveParentNode();

	}
	graph.RemoveNode(node);

}
int ClusterAbstraction::GetDistanceOver(Node * start, Node * goal)
{
	for (int i = 0; i < start->GetParentSize(); i++) {
		for (int j = 0; j < goal->GetParentSize(); j++) {
			if (start->GetParentIDAt(i) == start->GetParentIDAt(j)
				&& start->GetParentIDAt(i)->GetTheOtherNode(start->nodeNum)==goal) {
				return start->GetParentIDAt(i)->GetDistance();
			}
		}
	}
	return 0;
}
bool ClusterAbstraction::EdgeExist(Cluster & cluster, int nodeID1, int nodeID2, Graph * graph)
{
	bool ret = false;
	int number = 0;
	int edge_size = graph->EdgeSize();
	for (int i = 0; i < edge_size; i++) {
		if (graph->EdgeAt(i)->GetNodeNum1()->nodeNum == nodeID1 && graph->EdgeAt(i)->GetNodeNum2()->nodeNum == nodeID2
			|| graph->EdgeAt(i)->GetNodeNum1()->nodeNum == nodeID2 && graph->EdgeAt(i)->GetNodeNum2()->nodeNum == nodeID1)
		{
			ret = true;
		}
	
	}
	return ret;
}
void ClusterAbstraction::SetEntryClusterID()
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

void ClusterAbstraction::CreateGraph()
{
	SetNodesOnClusters(&graph);
}

void ClusterAbstraction::SetNodesOnClusters(Graph* graph)
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
			checkNum = NodeExist(cluster1, item.GetPosX(), item.GetPosY(), graph);
			if (checkNum == -1) {
				Node* node1 = new Node();
				numNode1 = graph->AddNode(node1);
				node1->SetPosition(item.GetPosX(), item.GetPosY());
				node1->SetClusterID(clusterID1);
				cluster1.AddNode(numNode1);
			//	LOG("Node %i Generated x = %i, y = %i", numNode1,node1->GetPositionX(), node1->GetPositionY() );

			}
			else {
				numNode1 = checkNum;
			}

			Cluster& cluster2 = GetCluster(clusterID2);
			checkNum = NodeExist(cluster2, item.GetPosX(), item.GetPosY()+1, graph);
			if (checkNum == -1) {
				Node* node2 = new Node();
				node2->SetPosition(item.GetPosX(), item.GetPosY() + 1);
				numNode2 = graph->AddNode(node2);
				node2->SetClusterID(clusterID2);
				cluster2.AddNode(numNode2);
			//	LOG("Node %i Generated x = %i, y = %i", numNode2, node2->GetPositionX(), node2->GetPositionY());

			}
			else {
				numNode2 = checkNum;
			}

			
			Edge* set = graph->AddEdge(new Edge(graph->GetNode(numNode1), graph->GetNode(numNode2), 1, INTER_EDGE));
			graph->GetNode(numNode1)->SetParent(set);
			graph->GetNode(numNode2)->SetParent(set);
		//	LOG("Inter Edge Generated on nodeOne = %i nodeTwo = %i", numNode1, numNode2);
		}

		break;
		case CLUSTER_VERTICAL:
		{
			Cluster& cluster1 = GetCluster(clusterID1);
			checkNum = NodeExist(cluster1, item.GetPosX(), item.GetPosY(), graph);

			if (checkNum == -1) {
				Node* node1 = new Node();
				numNode1 = graph->AddNode(node1);
				node1->SetPosition(item.GetPosX(), item.GetPosY());
				node1->SetClusterID(clusterID1);
				cluster1.AddNode(numNode1);
			//	LOG("Node %i Generated x = %i, y = %i", numNode1, node1->GetPositionX(), node1->GetPositionY());

			}
			else {
				numNode1 = checkNum;
			}

			Cluster& cluster2 = GetCluster(clusterID2);
			checkNum = NodeExist(cluster2, item.GetPosX()+1, item.GetPosY(), graph);
			if (checkNum == -1) {
				Node* node2 = new Node();
				node2->SetPosition(item.GetPosX() + 1, item.GetPosY());
				numNode2 = graph->AddNode(node2);
				node2->SetClusterID(clusterID2);
				cluster2.AddNode(numNode2);
			//	LOG("Node %i Generated x = %i, y = %i", numNode2,node2->GetPositionX(), node2->GetPositionY());

			}
			else {
				numNode2 = checkNum;
			}

			Edge* set = graph->AddEdge(new Edge(graph->GetNode(numNode1), graph->GetNode(numNode2), 1, INTER_EDGE));
			graph->GetNode(numNode1)->SetParent(set);
			graph->GetNode(numNode2)->SetParent(set);
		//	LOG("Inter Edge Generated on nodeOne = %i nodeTwo = %i", numNode1, numNode2);
		}
		break;
		default:
			break;
		}
	}
}
void ClusterAbstraction::CreateIntraEdges(Graph * graph)
{
	int new_size = clusterSize*clusterSize;
	uchar* temp_map = new uchar[new_size];
	std::vector<Node*> temp_nodes;

	for (int i = 0; i < clusters.size(); i++)
	{
		Cluster& item = clusters[i];
		int map_size = item.GetHeight()*item.GetWidth();

		if (new_size != map_size) {
			delete[] temp_map;
			new_size = map_size;
			temp_map = new uchar[new_size];
		}
		

		int x = 0;
		int y = 0;
		int node_number = 0;
	//	LOG("cluster %i", i);
		int cluster_pos_y = item.GetPosisitionY(), cluster_pos_x = item.GetPosisitionX();
		int cluster_height = item.GetHeight(), cluster_width = item.GetWidth();

		for (int i = cluster_pos_y; i < (cluster_pos_y + cluster_height); i++)
		{
			x = 0;
			for (int j = cluster_pos_x; j <(cluster_pos_x + cluster_width); j++)
			{
				temp_map[(y*cluster_width + x)] = GetValueMap(j, i);
				node_number = NodeExist(item, j, i, graph);
				if (node_number != -1) {
					Node* tmp_node = new Node();
					tmp_node->SetPosition(x, y);
					tmp_node->nodeNum = node_number;
					temp_nodes.push_back(tmp_node);
				}
				x++;
			}
			y++;
		}
		App->pathfinding->SetMap(cluster_width, cluster_height, temp_map);

		int distance=-1;
		int node_size = temp_nodes.size();
		for (int i = 0; i <node_size; i++)
		{
			for (int j = 0; j < node_size; j++)
			{
				if (i == j)
					continue;

				if (!EdgeExist(item, temp_nodes[i]->nodeNum, temp_nodes[j]->nodeNum, graph))
				{
					distance = App->pathfinding->CreatePath(temp_nodes[i], temp_nodes[j]);
					if (distance != -1) {

						Edge* set = graph->AddEdge(new Edge(graph->GetNode(temp_nodes[i]->nodeNum), graph->GetNode(temp_nodes[j]->nodeNum), distance, INTRA_EDGE));
						graph->GetNode(temp_nodes[i]->nodeNum)->SetParent(set);
						graph->GetNode(temp_nodes[j]->nodeNum)->SetParent(set);
						LOG("Intra Edge Generated on nodeOne = %i nodeTwo = %i", temp_nodes[i]->nodeNum, temp_nodes[j]->nodeNum);
					}
				}
			}

		}

		temp_nodes.clear();

	}

}
int ClusterAbstraction::CreateBFS(Node * from, Node * to)
{
	int ret = -1;
	
	std::queue<Node*> nodes1;
	std::queue<Node*> nodes2;
	bool look_queue1 = true;
	for (int i = 0; i < graph.GetNodesSize(); i++) 
	{
		graph.GetNodesAt(i)->ResetNode();
	}
	std::vector<Node*> goals;
	nodes1.push(from);
	Node* tmp = nodes1.front();
	bool goal = false;
	tmp->visited = true;
	int cycle = tmp->GetParentSize();
	while (goal != true)
	{
		if (look_queue1)
		{
			nodes1.pop();
			for (int i = 0; i < tmp->GetParentSize(); i++)
			{
				Node* objective_node = tmp->GetParentIDAt(i)->GetTheOtherNode(tmp->nodeNum);
				if (objective_node->visited != true)
				{
					if (objective_node == to) 
					{
						objective_node->SetTrack(tmp);
						objective_node->visited = true;
						goals.push_back(objective_node);
					}
					else {
						objective_node->SetTrack(tmp);

						objective_node->visited = true;
						nodes2.push(objective_node);
					}
				}
			}
			if (!nodes1.empty())
				tmp = nodes1.front();
			else {
				if (!goals.empty()) 
				{
					goal = true;
				}
				look_queue1 = false;
				tmp = nodes2.front();

			}
		}
		else
		{
			nodes2.pop();
			for (int i = 0; i < tmp->GetParentSize(); i++)
			{
				Node* objective_node = tmp->GetParentIDAt(i)->GetTheOtherNode(tmp->nodeNum);
				if (objective_node->visited != true)
				{
					if (objective_node == to) 
					{
						objective_node->SetTrack(tmp);
						objective_node->visited = true;
						goals.push_back(objective_node);
						goal = true;
					}
					else 
					{
						objective_node->SetTrack(tmp);

						objective_node->visited = true;
						nodes1.push(objective_node);
					}
				}
			}
			if (!nodes2.empty())
				tmp = nodes2.front();			
			else {
				if (!goals.empty())
				{
					goal = true;
				}
				look_queue1 = true;
				tmp = nodes1.front();

			}
		}
		
	}
	if (goal) {
		best_path.clear();
		Node* set_goal = nullptr;
		int distance = 0;
		int min_distance = 20000;
		Node* tmp_node1 = goals.back();
		Node* tmp_node2 = tmp_node1->GetTrackBack();
		while (!goals.empty()) {
			while (tmp_node1->nodeNum != from->nodeNum) {
				distance += GetDistanceOver(tmp_node1, tmp_node2);
				tmp_node1 = tmp_node1->GetTrackBack();
				tmp_node2 = tmp_node2->GetTrackBack();
			}
		//	distance += GetDistanceOver(tmp_node1, tmp_node2);
			if (distance < min_distance) {
				min_distance = distance;
				set_goal = goals.back();
			}
			goals.pop_back();
		}
		while (set_goal->nodeNum != from->nodeNum) {
			best_path.push_back(set_goal);
			set_goal = set_goal->GetTrackBack();
		}
		best_path.push_back(set_goal);
		ret = best_path.size();
	}
	return ret;

}
std::vector<Node*> ClusterAbstraction::GetBestPath()
{
	return best_path;
}
Node * ClusterAbstraction::GetBestPathAt(int index)
{
	return best_path[index];
}
Node* Graph::GetNode(int i) 
{
	if (i >= 0 && i < nodes.size()) {
		return nodes[i];
	}
	return 0;
}
Edge* Graph::AddEdge(Edge* edge)
{
	Edge* ret = nullptr;
	if (edge) {
		edges.push_back(edge);
		ret = edges.back();

	}
	return ret;
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

void Graph::RemoveNode(Node * node)
{
	int remove_id = node->nodeNum;
	if (nodes.back()->nodeNum == remove_id) {
		nodes.pop_back();
	}
}

void Graph::RemoveEdge(Edge* edge)
{
	//edges.
}



int Graph::EdgeSize()
{
	return edges.size();
}

int Graph::GetNodesSize()const
{
	return nodes.size();
}

Node * Graph::GetNodesAt(int index) const
{
	return nodes[index];
}

Edge * Graph::EdgeAt(int i)
{

	return edges[i];
}

Edge::Edge(Node* nodeNum1, Node* nodeNum2, int distance, EdgeType type) : nodeNum1(nodeNum1),nodeNum2(nodeNum2),distance(distance),type(type)
{

}
Edge::~Edge()
{
}

Node* Edge::GetNodeNum1()
{
	return nodeNum1;
}

Node* Edge::GetNodeNum2()
{
	return nodeNum2;
}

Node* Edge::GetTheOtherNode(int nodeID)
{
	Node* ret = nullptr;
	if (nodeNum1->nodeNum == nodeID)
		ret = nodeNum2;
	else
		ret = nodeNum1;
	return ret;
}

int Edge::GetDistance()
{
	return distance;
}

EdgeType Edge::GetEdgeType()
{
	return type;
}

Edge * Edge::RemoveParentNode()
{
	nodeNum1->RemoveParent(this);
	nodeNum2->RemoveParent(this);
	return this;
}

void Node::SetPosition(int x, int y)
{
	posX = x;
	posY = y;
}
void Node::SetClusterID(int id) 
{
	clusterID = id;
}
void Node::SetParent(Edge* edge)
{
	parent.push_back(edge);
}
void Node::SetTrack(Node * get)
{
	track_back = get;
}
int  Node::GetPositionX() 
{
	return posX;
}
int  Node::GetPositionY() 
{
	return posY;

}

Edge* Node::GetParentIDAt(int index)
{
	std::list<Edge*>::iterator item = parent.begin();
	int i = 0;
	while (item != parent.end())
	{
		if (i++ == index) {
			return item._Ptr->_Myval;
		}
		
		++item;
	}
	//return parent[index];
}

int Node::GetParentSize()
{
	return parent.size();
}

Node * Node::GetTrackBack()
{
	return track_back;
}

int Node::GetClusterId()
{
	return clusterID;
}

void Node::RemoveParent(Edge* edge)
{
	parent.remove(edge);
	//parent_ID.erase(node_id);
}

void Node::ResetNode()
{
	track_back = nullptr;
	visited = false;
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

#include "j1ClusterAbstraction.h"

Cluster::Cluster(int posX, int posY, int width, int height, int row, int column, int id):posX(posX),posY(posY),width(width), height(height),row(row),column(column),id(id)
{
}

Cluster::~Cluster()
{
}

j1ClusterAbstraction::j1ClusterAbstraction(j1Map * m, uint clusterSize):clusterSize(clusterSize)
{
	if (m->CreateWalkabilityMap(width, height, &map))
		SetMap(width, height, map);
	CreateClusters();
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


			column++;
		}
		row++;
	}
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

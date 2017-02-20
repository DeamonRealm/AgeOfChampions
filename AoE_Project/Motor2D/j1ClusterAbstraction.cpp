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
	for (int i = start; i < end; i++) {

	}
}

void j1ClusterAbstraction::CreateEntryVertical(int start, int end, int x, int row, int column)
{
}

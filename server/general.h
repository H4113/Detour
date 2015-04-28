#ifndef GENERAL_H
#define GENERAL_H

#include <vector>

struct Coordinates
{
	double longitude;
	double latitude;
};

struct Neighbor
{
	struct PathNode *node;
	struct Road *road;
};

struct PathNode
{
	Coordinates *point;
	std::vector<Neighbor> neighbors;
};

enum RoadDirection
{
	RD_BOTH_DIRECTIONS,
	RD_1_TO_2,
	RD_2_TO_1
};

struct Road
{
	double distance;
	std::vector<Coordinates> points; // Used only to send the road shape to the client
	Coordinates *point1;
	Coordinates *point2;
	RoadDirection direction;
};

#endif // GENERAL_H


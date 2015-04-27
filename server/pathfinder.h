#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "general.h"

enum RoadDirection
{
	RD_BOTH_DIRECTIONS,
	RD_1_TO_2,
	RD_2_TO_1
};

struct Road
{
	double distance;
	Coordinates *points; // Used only to send the road shape to the client
	unsigned int node1; // Index
	unsigned int node2; // Index
	RoadDirection direction;
};

void TestPathfinder(void);

#endif


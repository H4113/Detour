#include <iostream>

#include "pathfinder.h"

void TestPathfinder(void)
{
	const unsigned int W = 4;
	const unsigned int H = 4;
	const unsigned int NROADS = W * H * 4 - 2 * (W + H);
	const double DIST_ROAD = 1;

	Coordinates coord[W * H];
	Road roads[NROADS];
	unsigned iroad = 0;

	// Initialize graph
	
	for(unsigned int j = 0; j < H; ++j)
		for(unsigned int i = 0; i < W; ++i)
		{
			coord[i + j * W].longitude = i;
			coord[i + j * W].latitude = i;
			
			if(i > 0) // Left
			{
				Road *r = roads + (iroad++);
				r->distance = DIST_ROAD;
				r->point1 = i + j * W;
				r->point2 = r->point1 - 1;
				r->direction = RD_BOTH_DIRECTIONS;
			}
			if(i < W - 1) // Right
			{
				Road *r = roads + (iroad++);
				r->distance = DIST_ROAD;
				r->point1 = i + j * W;
				r->point2 = r->point1 + 1;
				r->direction = RD_BOTH_DIRECTIONS;
			}
			if(j > 0) // Up
			{
				Road *r = roads + (iroad++);
				r->distance = DIST_ROAD;
				r->point1 = i + j * W;
				r->point2 = r->point1 - W;
				r->direction = RD_BOTH_DIRECTIONS;
			}
			if(j < H - 1) // DOWN
			{
				Road *r = roads + (iroad++);
				r->distance = DIST_ROAD;
				r->point1 = i + j * W;
				r->point2 = r->point1 + W;
				r->direction = RD_BOTH_DIRECTIONS;
			}
		}

}


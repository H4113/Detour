#include <iostream>

#include "pathfinder.h"

// start and goal must exist in the nodes array
void PF_Astar(PathNode *start, PathNode *goal, std::vector<Coordinates> &result)
{
	result.clear();
}

void TestPathfinder(void)
{
	const unsigned int W = 4;
	const unsigned int H = 4;
	const unsigned int NROADS = W * H * 4 - 2 * (W + H);
	const double DIST_ROAD = 1;

	Coordinates coord[W * H];
	PathNode nodes[W * H];
	Road roads[NROADS];
	unsigned iroad = 0;

	// Initialize graph
	
	for(unsigned int j = 0; j < H; ++j)
		for(unsigned int i = 0; i < W; ++i)
		{
			unsigned int index = i + j * W;
			coord[index].longitude = i;
			coord[index].latitude = i;

			nodes[index].point = coord + index;
			
			if(i > 0) // Left
			{
				Road *r = roads + (iroad++);
				Neighbor nb;

				r->distance = DIST_ROAD;
				r->point1 = index;
				r->point2 = index - 1;
				r->direction = RD_BOTH_DIRECTIONS;
			
				nb.node = nodes + index - 1;
				nb.road = r;

				nodes[index].neighbors.push_back(nb);
			}
			if(i < W - 1) // Right
			{
				Road *r = roads + (iroad++);
				Neighbor nb;
				
				r->distance = DIST_ROAD;
				r->point1 = index;
				r->point2 = r->point1 + 1;
				r->direction = RD_BOTH_DIRECTIONS;
				
				nb.node = nodes + index + 1;
				nb.road = r;

				nodes[index].neighbors.push_back(nb);
			}
			if(j > 0) // Up
			{
				Road *r = roads + (iroad++);
				Neighbor nb;
				
				r->distance = DIST_ROAD;
				r->point1 = index;
				r->point2 = r->point1 - W;
				r->direction = RD_BOTH_DIRECTIONS;
			
				nb.node = nodes + index - W;
				nb.road = r;

				nodes[index].neighbors.push_back(nb);
			}
			if(j < H - 1) // DOWN
			{
				Road *r = roads + (iroad++);
				Neighbor nb;
				
				r->distance = DIST_ROAD;
				r->point1 = index;
				r->point2 = r->point1 + W;
				r->direction = RD_BOTH_DIRECTIONS;
				
				nb.node = nodes + index + W;
				nb.road = r;

				nodes[index].neighbors.push_back(nb);
			}
		}

}


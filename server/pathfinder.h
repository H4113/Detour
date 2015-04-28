#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <vector>
#include <deque>

#include "general.h"

double PF_EarthDistance(const PathNode *a, const PathNode *b);

	// start and goal must exist in the nodes array
bool PF_Astar(PathNode *start, PathNode *goal, const std::vector<PathNode*> &nodes, double (*distance)(const PathNode*, const PathNode*), double (*heuristic)(const PathNode*, const PathNode*), std::deque<PathNode*> &result);

void TestPathfinder(void);

#endif


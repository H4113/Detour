#include <iostream>
#include <cmath>

#include <set>
#include <map>

#include "pathfinder.h"

struct AstarNode
{
	PathNode *node;
	struct AstarNode *parent;
	double gScore;
	double fScore;
	
	AstarNode(PathNode *n, AstarNode *p, double g, double f):
		node(n),
		parent(p),
		gScore(g),
		fScore(f)
	{
	}
};

static AstarNode *minNode(const std::set<AstarNode*> &openSet)
{
	if(openSet.size())
	{
		std::set<AstarNode*>::const_iterator it = openSet.begin();
		AstarNode *node = *it;
		double minF = node->fScore;

		for(++it; it != openSet.end(); ++it)
			if((*it)->fScore < minF)
			{
				node = *it;
				minF = node->fScore;
			}

		return node;
	}

	return 0;
}

double PF_EarthDistance(const PathNode *a, const PathNode *b)
{
	double ta = M_PI * (90. - a->point->latitude) / 180.;
	double tb = M_PI * (90. - b->point->latitude) / 180.;

	return sqrt(ta * ta + tb * tb - 2. * ta * tb * cos(a->point->longitude - b->point->longitude));
}

bool PF_Astar(PathNode *start, PathNode *goal, const std::vector<PathNode*> &nodes, double (*heuristic)(const PathNode*, const PathNode*), std::deque<PathNode*> &result)
{
	std::set<PathNode*> closedSet;
	std::set<AstarNode*> openSet;
	std::map<PathNode*, AstarNode> nodeInfo;
	
	// Init
	nodeInfo.insert(std::pair<PathNode*, AstarNode>(start, AstarNode(start, 0, 0, heuristic(start, goal))));	
	openSet.insert(&(nodeInfo.begin()->second));

	for(std::vector<PathNode*>::const_iterator it = nodes.begin();
		it != nodes.end();
		++it)
	{
		if(*it != start)
		{
			nodeInfo.insert(std::pair<PathNode*, AstarNode>(*it, AstarNode(*it, 0, 0, 0)));
		}
	}

	// Loop
	while(openSet.size())
	{
		AstarNode *current = minNode(openSet);
		if(current->node == goal) // We're ok, ready to build the path
		{
			result.clear();
			while(current)
			{
				result.push_front(current->node);
				current = current->parent;
			}

			return true;
		}
		else
		{
			std::set<AstarNode*>::iterator it = openSet.find(current);
			if(it != openSet.end()) // Should be always true
			{
				openSet.erase(it);

				closedSet.insert(current->node);

				for(std::vector<Neighbor>::iterator itnb = current->node->neighbors.begin();
				    itnb != current->node->neighbors.end();
					++itnb)
				{
					if(closedSet.find(itnb->node) == closedSet.end())
					{
						PathNode *neighbor = itnb->node;
						std::map<PathNode*, AstarNode>::iterator itnbinfo = nodeInfo.find(neighbor);
						if(itnbinfo != nodeInfo.end()) // Should be always true
						{
							AstarNode *neighborAstar = &(itnbinfo->second);
							double tmpGScore = current->gScore + itnb->road->distance;
							if(tmpGScore < neighborAstar->gScore || openSet.find(neighborAstar) == openSet.end())
							{
								neighborAstar->parent = current;
								neighborAstar->gScore = tmpGScore;
								neighborAstar->fScore = tmpGScore + heuristic(neighbor, goal);
								
								// The following line won't affect openSet if it already contains neighbor
								openSet.insert(neighborAstar);
							}
						}
						else
						{
							std::cerr << "[PathFinder] An unknown error has occured" << std::endl;
							return false;
						}
					}
				}
			}
			else
			{
				std::cerr << "[PathFinder] An unkown error has occured" << std::endl;
				return false;
			}
		}
	}

	return false;
}



// Test features

static double testDistance(const PathNode *a, const PathNode *b)
{
	double diffLong = a->point->longitude - b->point->longitude;
	double diffLat = a->point->latitude - b->point->latitude;
	return diffLong * diffLong + diffLat * diffLat;
}

static double testHeuristic(const PathNode *a, const PathNode *b)
{
	return testDistance(a, b);
}

void TestPathfinder(void)
{
	const unsigned int W = 100;
	const unsigned int H = 100;
	const unsigned int NROADS = W * H * 4 - 2 * (W + H);
	const double DIST_ROAD = 1;

	Coordinates *coord = new Coordinates[W * H];
	std::vector<PathNode*> nodes;
	Road *roads = new Road[NROADS];
	unsigned iroad = 0;
	
	bool ret;
	std::deque<PathNode*> result;
	
	nodes.resize(W * H);

	// Initialize graph
	for(unsigned int i = 0; i < W * H; ++i)
		nodes[i] = new PathNode;

	for(unsigned int j = 0; j < H; ++j)
		for(unsigned int i = 0; i < W; ++i)
		{
			unsigned int index = i + j * W;
			PathNode *node = nodes[index];
			
			coord[index].longitude = i;
			coord[index].latitude = j;

			node->point = coord + index;
			
			if(i > 0) // Left
			{
				Road *r = roads + (iroad++);
				Neighbor nb;

				r->distance = DIST_ROAD;
				r->point1 = index;
				r->point2 = index - 1;
				r->direction = RD_BOTH_DIRECTIONS;
			
				nb.node = nodes[index - 1];
				nb.road = r;

				node->neighbors.push_back(nb);
			}
			if(i < W - 1) // Right
			{
				Road *r = roads + (iroad++);
				Neighbor nb;
				
				r->distance = DIST_ROAD;
				r->point1 = index;
				r->point2 = r->point1 + 1;
				r->direction = RD_BOTH_DIRECTIONS;
				
				nb.node = nodes[index + 1];
				nb.road = r;

				node->neighbors.push_back(nb);
			}
			if(j > 0) // Up
			{
				Road *r = roads + (iroad++);
				Neighbor nb;
				
				r->distance = DIST_ROAD;
				r->point1 = index;
				r->point2 = r->point1 - W;
				r->direction = RD_BOTH_DIRECTIONS;
			
				nb.node = nodes[index - W];
				nb.road = r;

				node->neighbors.push_back(nb);
			}
			if(j < H - 1) // DOWN
			{
				Road *r = roads + (iroad++);
				Neighbor nb;
				
				r->distance = DIST_ROAD;
				r->point1 = index;
				r->point2 = r->point1 + W;
				r->direction = RD_BOTH_DIRECTIONS;
				
				nb.node = nodes[index + W];
				nb.road = r;

				node->neighbors.push_back(nb);
			}
		}

	ret = PF_Astar(nodes[0], nodes[W * H - 1], nodes, PF_EarthDistance, result);

	if(ret)
	{
		unsigned int n = 1;
		
		std::cout << "I found a path!" << std::endl;
		std::cout << "Size: " << result.size() << std::endl;
		
		for(std::deque<PathNode*>::const_iterator it = result.begin(); it != result.end(); ++it)
		{
			std::cout << " n" << (n++) << ": (" << (*it)->point->longitude << ", " << (*it)->point->latitude << ")    " << (*it) << std::endl;	
		}
	}
	else
		std::cout << "No path..." << std::endl;

	delete [] coord;
	delete [] roads;
}


#include <iostream>
#include <cstdlib>
#include <cmath>

#include <set>
#include <map>

#include "pathfinder.h"
#include "import.h"
#include "utils.h"

PathFinder PathFinder::PF_instance;

struct AstarNode
{
	PathNode *node;
	struct AstarNode *parent;
	Road *roadToParent;
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

double PF_EarthDistance(const Coordinates &a, const Coordinates &b)
{
	double ta = M_PI * (90. - a.latitude) / 180.;
	double tb = M_PI * (90. - b.latitude) / 180.;

	return sqrt(ta * ta + tb * tb - 2. * ta * tb * cos(a.longitude - b.longitude));
}

double PF_EarthDistance(const PathNode *a, const PathNode *b)
{
	return PF_EarthDistance(*(a->point), *(b->point));
}

static double squareDist2(const Coordinates &a, const Coordinates &b)
{
	double m = a.longitude - b.longitude;
	double n = a.latitude - b.latitude;
	return m*m + n*n;
}

static bool operator==(const Coordinates &a, const Coordinates &b)
{
	const double EPSILON = 0.00001;
	return squareDist2(a, b) <= EPSILON;
}

PathFinder::PathFinder():
	heuristic(PF_EarthDistance),
	loaded(false),
	result(0)
{
}

PathFinder::~PathFinder()
{
	// Free roads
	
	// Free nodes

	// Free result
	freeResult();
}
		
void PathFinder::Load(void)
{
	if(!loaded)
	{
		std::cout << "Importing data..." << std::endl;
		ImportData(roads, nodes);
		std::cout << "Data imported!" << std::endl;
		loaded = true;
	}
}

bool PathFinder::Astar(const Coordinates &coordStart, const Coordinates &coordGoal)
{
	PathNode *start;
	PathNode *goal;
	std::set<PathNode*> closedSet;
	std::set<AstarNode*> openSet;
	std::map<PathNode*, AstarNode> nodeInfo;

	if(!(start = getClosestNode2(coordStart, &closestCoordStart)))
	{
		std::cerr << "[Pathfinder] Couldn't find a node close to start (" << coordStart.longitude << ", " << coordStart.latitude << ")" << std::endl;
		return false;
	}
	if(!(goal = getClosestNode2(coordGoal, &closestCoordGoal)))
	{
		std::cerr << "[Pathfinder] Couldn't find a node close to goal (" << coordGoal.longitude << ", " << coordGoal.latitude << ")" << std::endl;
		return false;
	}

	std::cout << "start: (" << start->point->longitude << ", " << start->point->latitude << ")" << std::endl;
	std::cout << "goal: (" << goal->point->longitude << ", " << goal->point->latitude << ")" << std::endl;
	
	// Init
	nodeInfo.insert(std::pair<PathNode*, AstarNode>(start, AstarNode(start, 0, 0, heuristic(start, goal))));	
	openSet.insert(&(nodeInfo.begin()->second));

	for(std::map<Coordinates, PathNode*>::const_iterator it = nodes.begin();
		it != nodes.end();
		++it)
	{
		if(it->second != start)
		{
			nodeInfo.insert(std::pair<PathNode*, AstarNode>(it->second, AstarNode(it->second, 0, 0, 0)));
		}
	}

	// Loop
	while(openSet.size())
	{
		AstarNode *current = minNode(openSet);
		if(current->node == goal) // We're ok, ready to build the path
		{
			ResultNode *rnode = 0;
			Road *road = 0;

			freeResult();

			while(current)
			{
				ResultNode *previous = rnode;

				rnode = new ResultNode;
				rnode->point = current->node->point;
				rnode->next = previous;
				rnode->roadToNext = road;

				road = current->roadToParent;

				current = current->parent;
			}

			result = rnode;

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
								neighborAstar->roadToParent = itnb->road;
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

	std::cerr << "[Pathfinder] Unable to find a path between the points..." << std::endl;

	return false;
}

bool PathFinder::BuildPath(std::vector<Coordinates> &path)
{
	bool startReached = false;
	bool goalReached = false;
	
	path.clear();

	if(!result)
		return false;	

	while(result && !goalReached)
	{
		path.push_back((*result->point));

		if(result->next != 0)
		{
			Road *road = result->roadToNext;
			if(road == 0)
			{
				result = result->next;
				continue;
			}

			if(road->point1 == result->point && road->point2 == result->next->point) // 1 -> 2
			{
				for(std::vector<Coordinates>::const_iterator it = road->points.begin();
					it != road->points.end() && !goalReached;
					++it)
				{
					if(startReached)
					{
						path.push_back(*it);
						if(*it == *closestCoordGoal)
							goalReached = true;
					}
					else if(*it == *closestCoordStart)
					{
						path.push_back(*it);
						startReached = true;
					}
				}
			}
			else if(road->point1 == result->next->point && road->point2 == result->point) // 2 -> 1
			{
				for(std::vector<Coordinates>::const_reverse_iterator it = road->points.rbegin();
					it != road->points.rend() && !goalReached;
					++it)
				{
					if(startReached)
					{
						path.push_back(*it);
						if(*it == *closestCoordGoal)
							goalReached = true;
					}
					else if(*it == *closestCoordStart)
					{
						path.push_back(*it);
						startReached = true;
					}
				}
			}
			else
			{
				std::cout << "[Pathfinder] Cannot build path..." << std::endl;
				std::cout << "(" << road->point1 << ", " << road->point2 << ") VS (" << result->point << ", " << result->next->point << ")" << std::endl;
				//return false;
			}
		}

		result = result->next;
	}

	return true;
}

PathNode *PathFinder::getClosestNode(const Coordinates &coord) const
{
	std::map<Coordinates, PathNode*>::const_iterator it = nodes.lower_bound(coord);
	if(it != nodes.end())
	{
		std::map<Coordinates, PathNode*>::const_iterator tmp = it++;
		std::map<Coordinates, PathNode*>::const_iterator itnext = it;
		if(itnext != nodes.end())
		{
			if(PF_EarthDistance(*(tmp->second->point), coord) < PF_EarthDistance(*(itnext->second->point), coord))
				return tmp->second;
			return itnext->second;
		}
		return it->second;
	}

	return 0;
}


static double minDistToRoad(const Coordinates &coord, const Road *road, Coordinates **closestNode, Coordinates **closestCoord)
{
	double distMin = squareDist2(coord, *(road->point1));
	double tmp = squareDist2(coord, *(road->point2));

	if(distMin <= tmp)
		*closestCoord = road->point1;
	else
	{
		*closestCoord = road->point2;
		distMin = tmp;
	}

	*closestNode = *closestCoord;

	for(std::vector<Coordinates>::const_iterator it = road->points.begin();
		it != road->points.end();
		++it)
	{
		tmp = squareDist2(coord, *it);
		if(tmp < distMin)
		{
			*closestCoord = (Coordinates*)&(*it);
			distMin = tmp;
		}
	}

	return distMin;
}

PathNode *PathFinder::getClosestNode2(const Coordinates &coord, Coordinates **closestCoord) const
{
	std::map<unsigned int, Road*>::const_iterator it = roads.begin();
	if(it != roads.end())
	{
		Coordinates *closestNode;
		double distMin = minDistToRoad(coord, it->second, &closestNode, closestCoord);

		std::map<Coordinates, PathNode*>::const_iterator itnode;

		for(; it != roads.end(); ++it)
		{
			Coordinates *tmpNode;
			Coordinates *tmpCoord;
			double tmp = minDistToRoad(coord, it->second, &tmpNode, &tmpCoord);
			if(tmp < distMin)
			{
				closestNode = tmpNode;
				*closestCoord = tmpCoord;

				distMin = tmp;
			}
		}

		itnode = nodes.find(*closestNode);
		if(itnode != nodes.end()) // Should be always true
		{
			return itnode->second;
		}
		else // Perform a manual search
		{
			PathNode *pnode;
			
			itnode = nodes.begin();
			pnode = itnode->second;
			distMin = squareDist2(*closestNode, *(itnode->second->point));

			for(; itnode != nodes.end(); ++itnode)
			{
				double tmp = squareDist2(*closestNode, *(itnode->second->point));
				if(tmp < distMin)
				{
					pnode = itnode->second;
					distMin = tmp;
				}
			}

			return pnode;
		}
	}

	return 0;
}

void PathFinder::freeResult(void)
{
	ResultNode *tmp;
	while(result)
	{
		tmp = result;
		result = result->next;
		delete tmp;
	}
}

// Test features

/*
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

static void testInitRoad(Road *road)
{
	const unsigned int MAX_INTERM_POINTS = 4;

	unsigned int nbPoints = rand() % (MAX_INTERM_POINTS+1);
	for(unsigned int i = 1; i <= nbPoints; ++i)
	{
		double a = (double) i / (double) (nbPoints+1);
		Coordinates coord =
		{
			(1.f-a) * road->point1->longitude + a * road->point2->longitude,
			(1.f-a) * road->point1->latitude + a * road->point2->latitude
		};

		road->points.push_back(coord);
	}
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
	ResultNode *result;
	
	nodes.resize(W * H);

	srand(time(0));

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
				r->point1 = node->point;
				r->point2 = coord + index - 1;
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
				r->point1 = node->point;
				r->point2 = coord + index + 1;
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
				r->point1 = node->point;
				r->point2 = coord + index - W;
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
				r->point1 = node->point;
				r->point2 = coord + index + W;
				r->direction = RD_BOTH_DIRECTIONS;
			
				nb.node = nodes[index + W];
				nb.road = r;

				node->neighbors.push_back(nb);
			}
		}
	
	for(unsigned int i = 0; i < NROADS; ++i)
		testInitRoad(roads + i);

	ret = PF_Astar(nodes[0], nodes[W * H - 1], nodes, PF_EarthDistance, &result);

	if(ret)
	{
		std::vector<Coordinates> path;
		
		std::cout << "I found a path!" << std::endl;
		std::cout << "Building path..." << std::endl;

		if(PF_BuildPath(result, path))
		{
			unsigned int n = 1;
			
			std::cout << "Path size: " << path.size() << std::endl;
			for(std::vector<Coordinates>::const_iterator it = path.begin();
				it != path.end();
				++it)
			{
				std::cout << " n" << (n++) << ": (" << it->longitude << ", " << it->latitude << ")" << std::endl;	
			}
		}
		
		PF_FreeResult(result);
	}
	else
		std::cout << "No path..." << std::endl;

	delete [] coord;
	delete [] roads;
}
*/

void TestPathfinderRealData(void)
{
	const Coordinates COORD_START = {45.7825076, 4.8736838};
	const Coordinates COORD_GOAL = {45.6803042752, 4.92207816575};
	
	std::vector<Coordinates> path;
	
	if(PF_FindPath(COORD_START, COORD_GOAL, path))
	{
		unsigned int n = 1;
		for(std::vector<Coordinates>::const_iterator it = path.begin();
			it != path.end();
			++it)
		{
			std::cout << " n" << (n++) << ": (" << it->longitude << ", " << it->latitude << ")" << std::endl;	
		}
	}
}

bool PF_FindPath(const Coordinates &coordStart, const Coordinates &coordGoal, std::vector<Coordinates> &path)
{
	PathFinder::PF_instance.Load();
	if(PathFinder::PF_instance.Astar(coordStart, coordGoal))
	{
		std::cout << "I found a path!" << std::endl;
		std::cout << "Building path..." << std::endl;
		if(PathFinder::PF_instance.BuildPath(path))
		{
			std::cout << "Path built!" << std::endl;
			std::cout << "Path size: " << path.size() << std::endl;
		}
		else
		{
			std::cout << "The path cannot be built" << std::endl;
			return false;
		}
		return true;
	}
	return false;
}


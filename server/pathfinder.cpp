#include <iostream>
#include <cstdlib>
#include <cmath>

#include <set>
#include <map>

#include "pathfinder.h"
#include "import.h"
#include "utils.h"
#include "tourism.h"

double (*PathFinder::heuristic)(const PathNode*, const PathNode*) = PF_EarthDistance;
bool PathFinder::loaded = false;
std::map<unsigned int, Road*> PathFinder::roads;
std::map<Coordinates, PathNode*> PathFinder::nodes;

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

void FreePathResult(Path *path)
{
	ResultNode *result = path->result;
	ResultNode *tmp;
	while(result)
	{
		tmp = result;
		result = result->next;
		delete tmp;
	}
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

static bool operator==(const Coordinates &a, const Coordinates &b)
{
	const double EPSILON = 0.00001;
	return squareDist2(a, b) <= EPSILON;
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

PathFinder::PathFinder()
{
}

PathFinder::~PathFinder()
{
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

bool PathFinder::Astar(const Coordinates &coordStart, const Coordinates &coordGoal, Path &resultPath)
{
	PathNode *start;
	PathNode *goal;
	std::set<PathNode*> closedSet;
	std::set<AstarNode*> openSet;
	std::map<PathNode*, AstarNode> nodeInfo;

	if(!(start = getClosestNode(coordStart, &(resultPath.closestCoordStart))))
	{
		std::cerr << "[Pathfinder] Couldn't find a node close to start (" << coordStart.longitude << ", " << coordStart.latitude << ")" << std::endl;
		return false;
	}
	if(!(goal = getClosestNode(coordGoal, &(resultPath.closestCoordGoal))))
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

			resultPath.result = rnode;
			resultPath.realStart = &coordStart;
			resultPath.realGoal = &coordGoal;

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

bool PathFinder::BuildPath(const Path &resultPath, std::vector<Coordinates> &path)
{
	bool startReached = false;
	bool goalReached = false;
	
	ResultNode *result = resultPath.result;

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
						if(*it == *(resultPath.closestCoordGoal))
							goalReached = true;
					}
					else if(*it == *(resultPath.closestCoordStart))
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
						if(*it == *(resultPath.closestCoordGoal))
							goalReached = true;
					}
					else if(*it == *(resultPath.closestCoordStart))
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

PathNode *PathFinder::getClosestNode(const Coordinates &coord, Coordinates **closestCoord)
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

// Test features

void TestPathfinderRealData(Database *db)
{
	const Coordinates COORD_START = {45.7825076, 4.8736838};
	const Coordinates COORD_GOAL = {45.6803042752, 4.92207816575};
	const double MAX_DEVIATION = 1000.;

	std::vector<Coordinates> path;
	std::vector<TouristicPlace> places;

	if(PF_FindPath(COORD_START, COORD_GOAL, MAX_DEVIATION, path, places, db))
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

bool PF_FindPath(const Coordinates &coordStart, const Coordinates &coordGoal, double maxDeviation, std::vector<Coordinates> &path, std::vector<TouristicPlace> &places, Database *database)
{
	Path resultPath;

	PathFinder::Load();
	if(PathFinder::Astar(coordStart, coordGoal, resultPath))
	{
		std::cout << "I found a path!" << std::endl;
		std::cout << "Building path..." << std::endl;
		if(PathFinder::BuildPath(resultPath, path))
		{
			TouristicFilter filter = {true, false, false};
			std::vector<Coordinates> finalPath;

			std::cout << "Path built!" << std::endl;
			std::cout << "Path size: " << path.size() << std::endl;
			std::cout << "Searching for a touristic path..." << std::endl;

			if(BuildTouristicPath(resultPath, path, finalPath, places, filter, maxDeviation, database))
			{
				path = finalPath;
				FreePathResult(&resultPath);
			}
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


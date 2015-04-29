#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <vector>
#include <map>

#include "general.h"

struct ResultNode
{
	Coordinates *point;
	struct ResultNode *next;
	Road *roadToNext;
};

double PF_EarthDistance(const Coordinates &a, const Coordinates &b);
double PF_EarthDistance(const PathNode *a, const PathNode *b);

class PathFinder
{
	public:
		PathFinder();
		virtual ~PathFinder();
		
		void Load(void);
		
		bool Astar(const Coordinates &coordStart, const Coordinates &coordGoal);
		bool BuildPath(std::vector<Coordinates> &path);
	
	protected:
		PathNode *getClosestNode(const Coordinates &coord);

	private:
		double (*heuristic)(const PathNode*, const PathNode*);

		std::map<unsigned int, Road*> roads;
		std::map<Coordinates, PathNode*> nodes;
		ResultNode *result;
};

void TestPathfinder(void);
void TestPathfinderRealData(void);

#endif


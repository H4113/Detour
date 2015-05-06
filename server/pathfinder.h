#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <vector>
#include <map>

#include "general.h"

class Database;

struct ResultNode
{
	Coordinates *point;
	struct ResultNode *next;
	Road *roadToNext;
};

struct Path
{
	ResultNode *result;
	const Coordinates *realStart;
	const Coordinates *realGoal;
	Coordinates *closestCoordStart;
	Coordinates *closestCoordGoal;
};

void FreePathResult(Path *path);

double PF_EarthDistance(const Coordinates &a, const Coordinates &b);
double PF_EarthDistance(const PathNode *a, const PathNode *b);

class PathFinder
{
	public:
		virtual ~PathFinder();
		
		static void Load(void);
		static bool Astar(const Coordinates &coordStart, const Coordinates &coordGoal, Path &resultPath);
		
		static bool BuildPath(const Path &resultPath, std::vector<Coordinates> &path);

	protected:
		PathFinder();
		static PathNode *getClosestNode(const Coordinates &coord, Coordinates **closestCoord);

	private:
		static double (*heuristic)(const PathNode*, const PathNode*);
		static bool loaded;
		static std::map<unsigned int, Road*> roads;
		static std::map<Coordinates, PathNode*> nodes;
};

void TestPathfinder(void);
void TestPathfinderRealData(Database *db);

bool PF_FindPath(const Coordinates &coordStart, const Coordinates &coordGoal, int duration, std::vector<Coordinates> &path, std::vector<struct TouristicPlace> &places, Database *database);

#endif


#include <iostream>
#include <cstdlib>

#include "utils.h"
#include "general.h"
#include "pathfinder.h"

bool operator<(const Coordinates &a, const Coordinates &b)
{
	if(a.longitude < b.longitude)
		return true;
	return a.latitude < b.latitude;
}

void error(const char *msg)
{
	std::cerr << msg << std::endl;
	exit(1);
}

PathNode *GetClosestNode(const std::map<Coordinates, PathNode*> &nodes, const Coordinates &coord)
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


#ifndef NETWORK_H
#define NETWORK_H

#include "general.h"

struct PathRequest
{
	Coordinates pointA;
	Coordinates pointB;
};

struct PathAnswer
{
	PathNode* first;
};

#endif // NETWORK_H

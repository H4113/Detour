#ifndef NETWORK_H
#define NETWORK_H

#include "general.h"

#define PORT_NUMBER 6666

struct PathRequest
{
	Coordinates pointA;
	Coordinates pointB;
};

struct PathAnswer
{
	PathNode* first;
};

void start(void);

#endif // NETWORK_H

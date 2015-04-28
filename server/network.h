#ifndef NETWORK_H
#define NETWORK_H

#include "general.h"

#define NET_PORT_NUMBER 6666
#define NET_SEPARATOR (char*)'A'

struct PathRequest
{
	Coordinates pointA;
	Coordinates pointB;
};

struct PathAnswer
{
	PathNode* first;
};

void startServer(void);

#endif // NETWORK_H

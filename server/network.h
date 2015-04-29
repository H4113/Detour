#ifndef NETWORK_H
#define NETWORK_H

#include "general.h"

#define NET_PORT_NUMBER 6666
#define NET_SEPARATOR (char*)'A'

struct PathR
{
	Coordinates pointA;
	Coordinates pointB;
};

struct PathAnswer
{
	PathNode* first;
};

union PathRequest
{
	char buffer[20];
	struct
	{
		int type;
		int junk;
		PathR path;
	};
};

void startServer(void);

#endif // NETWORK_H

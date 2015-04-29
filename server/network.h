#ifndef NETWORK_H
#define NETWORK_H

#include "general.h"

#include <stdint.h>

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
	char buffer[40];
	struct
	{
		int16_t type;
		int16_t junk;
		int32_t junk2;
		PathR path;
	};
};

void startServer(void);

#endif // NETWORK_H

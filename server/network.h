#ifndef NETWORK_H
#define NETWORK_H

#include "general.h"
#include "database.h"

#include <stdint.h>

#define NET_PORT_NUMBER 6666
#define NET_SEPARATOR (char*)'A'

enum PacketType
{
	PT_PathQuery,
	PT_PathAnswer,
	PT_TourismAnswer
};

struct SocketAndDB
{
	int clientSocket;
	Database *database;
};

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

void startServer(Database *db);

#endif // NETWORK_H

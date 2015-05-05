#ifndef NETWORK_H
#define NETWORK_H

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include "general.h"
#include "database.h"

#include <stdint.h>

#define NET_PORT_NUMBER 6666
#define NET_SEPARATOR (char*)'A'

typedef websocketpp::server<websocketpp::config::asio> server;

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

class WebServer
{
	public:
		WebServer(Database *db);
		virtual ~WebServer();
		void on_message(websocketpp::connection_hdl hdl, server::message_ptr msg);
	protected:
		Database *db;
		server webserver;
};

void startServer(Database *db);


std::string buildData(void* attr);

#endif // NETWORK_H

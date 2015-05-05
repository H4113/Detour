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

void startServer(Database *db);

void on_message(server* s, websocketpp::connection_hdl hdl, server::message_ptr msg) {
    std::cout << msg->get_payload() << std::endl;
    std::cout << "on_message called with hdl: " << hdl.lock().get()
          << " and message: " << msg->get_payload()
          << std::endl;
    //s->send(hdl, msg->get_payload(), msg->get_opcode());
}


#endif // NETWORK_H

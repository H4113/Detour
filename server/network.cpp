#include "network.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void start(void)
{
	int serverSocket, 
		clientSocket, 
		portNumber;
	struct sockaddr_in serverAddress,
		clientAddress;
	char buffer[256];
	int n;
	socklen_t clientLength;

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(serverSocket < 0) 
		error("ERROR opening server socket");

	// Maybe parameter ?
	portNumber = PORT_NUMBER;

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(portNumber);

	if(bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) 
		error("ERROR on binding server socket to server address");

	listen(serverSocket, SOMAXCONN);

	while(1)
	{
		clientLength = sizeof(clientAddress);
		clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddress, &clientLength);
		if(clientSocket < 0) 
			error("ERROR on accepting client socket");
		n = read(clientSocket,buffer,255);
		if (n < 0) 
			error("ERROR reading from client socket");
		printf("Here is the message: %s\n",buffer);
	}
}

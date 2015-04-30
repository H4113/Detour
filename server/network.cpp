#include "network.h"
#include "utils.h"
#include "general.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

static void splitArray(char* c, const char* s, char* first, char* second)
{
	int i = 0;
	second = strpbrk(c, s);
	if(second == 0) // null
		error("ERROR, wrong request");
	++second;
	printf("%s", c);
	while(*c && *c != *s)
	{
		printf("%s\n", c);
		first[i++] = *c++;
		printf("i : %d ¬¬ c : %s ¬¬ first : %s  \n", i, c, first);
	}
	printf("second : %s\n", second);
}

static void *clientRoutine(void* clientSocket)
{
	printf("\n\tB\n");
	int cs = *(reinterpret_cast<int*>(clientSocket));
	PathRequest pr;
	char first[256], 
		second[256];
	int n;
	bzero(first,256);
	bzero(second,256);
	bzero(pr.buffer,40);

	n = read(cs, pr.buffer, 40);
	if (n < 0)
		error("ERROR reading from client socket");
	printf("Here is the request from the client: %d %d\n",pr.type,pr.junk);
	printf("%lf %lf %lf %lf\n", pr.path.pointA.longitude, pr.path.pointA.latitude, pr.path.pointB.longitude, pr.path.pointB.latitude);

	// PROCESS HERE

	char* answer = "LAULAULAUWL";
	n = write(cs, answer, strlen(answer));
	printf("sent : %d\n", n);

	printf("\n\tE\n");

	close(cs);
	pthread_exit(NULL);
}

void startServer(void)
{
	pthread_t thread;
	int serverSocket, 
		clientSocket, 
		portNumber;
	struct sockaddr_in serverAddress,
		clientAddress;
	socklen_t clientLength;

	serverSocket = socket(PF_INET, SOCK_STREAM, 0);
	if(serverSocket < 0) 
		error("ERROR opening server socket");

	// Maybe parameter ?
	portNumber = NET_PORT_NUMBER;

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

		pthread_create(&thread, NULL, clientRoutine, (void *)&clientSocket);
	}

	pthread_exit(NULL);
}

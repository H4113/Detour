#include "network.h"
#include "utils.h"
#include "general.h"
#include "pathfinder.h"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <signal.h>

static int portNumber;

void siginthandler(int)
{
	close(portNumber);
	exit(1);
}

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
	//?
	//printf("\n\tB - %d\n", RLIMIT_FSIZE);
	int cs = *(reinterpret_cast<int*>(clientSocket));
	PathRequest pr;
	char first[256], 
		second[256];
	int n;
	bzero(first,256);
	bzero(second,256);
	bzero(pr.buffer,40);

	// REQUEST
	n = read(cs, pr.buffer, 40);
	if (n < 0)
		error("ERROR reading from client socket");
	printf("Here is the request from the client: %d %d\n",pr.type,pr.junk);
	printf("%.30f %.30f %.30f %.30f\n", pr.path.pointA.longitude, pr.path.pointA.latitude, pr.path.pointB.longitude, pr.path.pointB.latitude);

	// BUILD PATH
	std::vector<Coordinates> path;
	if(PF_FindPath(pr.path.pointA, pr.path.pointB, path))
	{
		// ANSWER !!!!
		int32_t type = 1;
		int32_t size = 2 * sizeof(int32_t) + sizeof(double) * path.size() * 2;
		int32_t nbDouble = path.size();
		int8_t* answer = new int8_t[size];
		int8_t* ptr;
		
		std::cout << path.size() << std::endl;
		
		memcpy(answer, &type, sizeof(int32_t));
		memcpy(answer + sizeof(int32_t), (char*) &(size), sizeof(int32_t));

		ptr = answer + 2 * sizeof(int32_t);

		for(std::vector<Coordinates>::iterator it = path.begin();
			it != path.end();
			++it, ptr += 2 * sizeof(double)) 
		{
			memcpy(ptr, &(it->longitude), sizeof(double));
			memcpy(ptr + sizeof(double), &(it->latitude), sizeof(double));
		}

		// std::ofstream myfile;
		// myfile.open ("buff.bin");
		// for(int i = 0; i < size; ++i) {
		// 	myfile << answer[i];
		// }
		// myfile.close();

		n = write(cs, answer, size);
		std::cout << "sent : " << n << " " << (size * sizeof(int8_t)) << std::endl;
		delete[] answer;
	} else {
		// Send error?
	}

	printf("\n\tE\n");

	close(cs);
	delete clientSocket;
	pthread_exit(NULL);
}

void startServer(void)
{
	pthread_t thread;
	int serverSocket, 
		clientSocket;
	int* clso;
	struct sockaddr_in serverAddress,
		clientAddress;
	socklen_t clientLength;

	signal(SIGINT, siginthandler);

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

		clso = new int;
		*clso = clientSocket;
		if(pthread_create(&thread, NULL, clientRoutine, (void *)clso) < 0)
			error("ERROR creating thread");
	}

	pthread_exit(NULL);
}

#include "network.h"
#include "utils.h"
#include "general.h"
#include "pathfinder.h"
#include "tourism.h"

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
#include <string.h>

static int portNumber;

void siginthandler(int)
{
	close(portNumber);
	exit(1);
}

static void paramFilter(PathRequest pr)
{
	bool patrimony = pr.junk & 1;
	bool gastronomy = pr.junk & 2;
	bool accomodation = pr.junk & 4;
	std::cout << patrimony << " " << gastronomy << " " << accomodation << std::endl;
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

	if(pr.type == PT_PathQuery) 
	{
		paramFilter(pr);

		// BUILD PATH
		std::vector<Coordinates> path;
		std::vector<TouristicPlace> touristicPlaces;
		if(PF_FindPath(pr.path.pointA, pr.path.pointB, path, touristicPlaces))
		{
			int16_t sizeType, sizeTypeDetail, sizeName, sizeAddress, sizeWorkingHours;
			// ANSWER !!!!
			int32_t type = 1;
			// type + size + path_size + touristic_size + ... path ...
			int32_t size = 4 * sizeof(int32_t) + sizeof(double) * path.size() * 2;
			//int32_t nbDouble = path.size();

			for(std::vector<TouristicPlace>::iterator it = touristicPlaces.begin();
				it != touristicPlaces.end();
				++it)
			{
				sizeType = it->type.size();
				sizeTypeDetail = it->typeDetail.size();
				sizeName = it->name.size();
				sizeAddress = it->address.size();
				sizeWorkingHours = it->workingHours.size();
				size += sizeof(int16_t) * 5 + (sizeType + sizeTypeDetail + sizeWorkingHours + sizeAddress + sizeName) *
					sizeof(char) + 2 * sizeof(double);
			}

			int8_t* answer = new int8_t[size];
			int8_t* ptr;
			int32_t pathSize = path.size();
			int32_t touriSize = touristicPlaces.size();
			
			std::cout << pathSize << " " << touriSize << std::endl;
			
			memcpy(answer, &type, sizeof(int32_t));
			memcpy(answer + sizeof(int32_t), &size, sizeof(int32_t));
			memcpy(answer + sizeof(int32_t) * 2, &pathSize, sizeof(int32_t));
			memcpy(answer + sizeof(int32_t) * 3, &touriSize, sizeof(int32_t));

			ptr = answer + 4 * sizeof(int32_t);

			for(std::vector<Coordinates>::iterator it = path.begin();
				it != path.end();
				++it, ptr += 2 * sizeof(double)) 
			{
				memcpy(ptr, &(it->longitude), sizeof(double));
				memcpy(ptr + sizeof(double), &(it->latitude), sizeof(double));
			}

			// Add touristic places
			for(std::vector<TouristicPlace>::iterator it = touristicPlaces.begin();
				it != touristicPlaces.end();
				++it)
			{
				sizeType = it->type.size();
				sizeTypeDetail = it->typeDetail.size();
				sizeName = it->name.size();
				sizeAddress = it->address.size();
				sizeWorkingHours = it->workingHours.size();
				memcpy(ptr, &sizeType, sizeof(int16_t));
				memcpy(ptr + sizeof(int16_t), &sizeTypeDetail, sizeof(int16_t));
				memcpy(ptr + sizeof(int16_t) * 2, &sizeName, sizeof(int16_t));
				memcpy(ptr + sizeof(int16_t) * 3, &sizeAddress, sizeof(int16_t));
				memcpy(ptr + sizeof(int16_t) * 4, &sizeWorkingHours, sizeof(int16_t));

				ptr += sizeof(int16_t) * 5;

				memcpy(ptr, &(it->location.longitude), sizeof(double));
				memcpy(ptr + sizeof(double), &(it->location.latitude), sizeof(double));

				ptr += 2 * sizeof(double);

				memcpy(ptr, it->type.c_str(), sizeType * sizeof(char));
				ptr += sizeType * sizeof(char);
				memcpy(ptr, it->typeDetail.c_str(), sizeTypeDetail * sizeof(char));
				ptr += sizeTypeDetail * sizeof(char);
				memcpy(ptr, it->name.c_str(), sizeName * sizeof(char));
				ptr += sizeName * sizeof(char);
				memcpy(ptr, it->address.c_str(), sizeAddress * sizeof(char));
				ptr += sizeAddress * sizeof(char);
				memcpy(ptr, it->workingHours.c_str(), sizeWorkingHours * sizeof(char));
				ptr += sizeWorkingHours * sizeof(char);
			}

			std::cout << "last : " << path[path.size() - 1].longitude << " " << path[path.size() - 1].latitude << std::endl; 

			n = write(cs, answer, size);
			std::cout << "sent : " << n << " " << (size * sizeof(int8_t)) << std::endl;

			delete[] answer;
		} else {
			// Send error?
		}
	} // else send error?

	printf("\n\tE\n");

	close(cs);
	delete clientSocket;
	pthread_exit(NULL);
}

void startServer(void)
{
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
		pthread_t thread;
		if(pthread_create(&thread, NULL, clientRoutine, (void *)clso) < 0) {
			close(*clso);
			delete clso;
			error("ERROR creating thread");
		}
	}

	pthread_exit(NULL);
}

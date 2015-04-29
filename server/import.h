#ifndef IMPORT_H
#define IMPORT_H

#include "general.h"

#define PATH_FILE (char*)"../data/"
#define ROADS_FILE (char*)"roads.out"
#define NODES_FILE (char*)"nodes.out"

/*
*	Use this function to generate the map's nodes
*/
void ImportNodes();

/*
*	Use this function to generate the map's roads 
*/
void ImportRoads();

/*
*	This function generate a road from a string
*/
void ParseRoadLine(std::string);

/*
*	No quite a finished road -> point1 and point2 aren't Coordinates yet
*/
struct ImportedRoad
{
	double distance;
	std::vector<Coordinates> points; // Used only to send the road shape to the client
	unsigned int point1;
	unsigned int point2;
	RoadDirection direction;
};

#endif // IMPORT_H

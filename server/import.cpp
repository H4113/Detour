#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <iterator>

#include "import.h"
#include "utils.h"

void ImportData(std::map<unsigned int, Road*> &roads, std::map<Coordinates, PathNode*> &nodes) {
	std::map<unsigned int, ImportedRoad*> importedRoads;
	ImportRoads(importedRoads);
	ImportNodes(nodes, roads, importedRoads);
}

void ImportRoads(std::map<unsigned int, ImportedRoad*> &roads) {
	roads.clear();

	std::string line;
	char str[20] = "";

	strcat(str,PATH_FILE);
	strcat(str, ROADS_FILE);
	std::ifstream file (str);


	if(file.is_open()) {
		
		while(getline(file, line)) {
			std::istringstream is (line);
			std::string part;
			
			ImportedRoad* road = new ImportedRoad();

			unsigned int i = 0, id = 0, id_start = 0, id_end = 0, nbr_points = 0;
			double distance = 0;
			std::vector<Coordinates> points;
			RoadDirection direction = RD_BOTH_DIRECTIONS;
			Coordinates point = {0,0};

			while(getline(is, part,',')) {
				switch (i) {
					case 0:
						id = ParseUint(part);
						break;
					case 1:
						distance = ParseDouble(part);
						break;
					case 2:
						id_start = ParseUint(part);
						break;
					case 3:
						id_end = ParseUint(part);
						break;
					case 4:
						nbr_points = ParseUint(part);
						break;
					default:
						//Last info
						if(i == 5+nbr_points*2) {
							direction = (RoadDirection)(ParseUint(part));
							break;
						}
						else {
							if(i%2) {
								point.latitude = 0;
								point.longitude = ParseDouble(part);
							}
							else {
								point.latitude = ParseDouble(part);
								points.push_back(point);
							}
							break;
						}
				}
				i++;
			}
			road->distance = distance;
			road->points = points;
			road->point1 = id_start;
			road->point2 = id_end;
			road->direction = direction;

			roads.insert(std::pair<unsigned int,ImportedRoad*>(id,road));
		}
	}

	file.close();
}

void ImportNodes(std::map<Coordinates, PathNode*> &nodes, std::map<unsigned int, Road*> &roads, 
	const std::map<unsigned int, ImportedRoad*> &importedRoads) {
	nodes.clear();
	roads.clear();

	std::string line;
	char str[20] = "";

	strcat(str,PATH_FILE);
	strcat(str, NODES_FILE);
	std::ifstream file(str);

	std::map<unsigned int, PathNode*> nodesNoNeighbor;

	if(file.is_open()) {
		
		while(getline(file, line)) {
			std::istringstream is (line);
			std::string part;

			Coordinates* point = new Coordinates();
			unsigned int i = 0, id = 0, id_road = 0;

			PathNode* node = new PathNode();
			
			while(getline(is, part,',')) {
				switch (i) {
					case 0:
						id = ParseUint(part);
						break;
					case 1:
						point->longitude = ParseDouble(part);
						break;
					case 2:
						point->latitude = ParseDouble(part);
						break;
					case 3:
						break;
					default:
						if(i%2) {
							Road* road = new Road();

							id_road = ParseUint(part);
							std::map<unsigned int, ImportedRoad*>::const_iterator itRoad = importedRoads.find(id_road);
							
							road->distance = itRoad->second->distance;
							road->points = itRoad->second->points;
							road->direction = itRoad->second->direction;
							
							roads.insert(std::pair<unsigned int,Road*>(id_road,road));
						}
				}
				i++;
			}
			node->point = point;
			nodesNoNeighbor.insert(std::pair<unsigned int,PathNode*>(id,node));

			nodes.insert(std::pair<Coordinates, PathNode*>(*point,node));
		}
	}
	file.close();

	//Neighbors generation
	std::map<unsigned int, ImportedRoad*>::const_iterator itIR;
	PathNode *node_start, *node_end;
	Neighbor n1, n2;
	Road* r;

	for(itIR = importedRoads.begin(); itIR != importedRoads.end(); ++itIR) {
		node_start = nodesNoNeighbor.find(itIR->second->point1)->second;
		node_end = nodesNoNeighbor.find(itIR->second->point2)->second;

		r = roads.find(itIR->first)->second;
		r->point1 = node_start->point;
		r->point2 = node_end->point;

		n1.node = node_start;
		n1.road = r;

		n2.node = node_end;
		n2.road = r;

		node_start->neighbors.push_back(n2);
		node_end->neighbors.push_back(n1);
	}
}

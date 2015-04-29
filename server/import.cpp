#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <iterator>

#include "import.h"
#include "utils.h"

void ImportRoads(std::map<unsigned int, ImportedRoad*> &roads) {
	std::string line;
	char str[20] = "";

	strcat(str,PATH_FILE);
	strcat(str, ROADS_FILE);
	std::ifstream file (str);


	if(file.is_open()) {
		
		while(getline(file, line)) {
			std::istringstream is (line);
			std::string part;
			unsigned int i = 0;
			
			ImportedRoad* road = new ImportedRoad();

			unsigned int id = 0;
			double distance = 0;
			unsigned int id_start = 0;
			unsigned int id_end = 0;
			unsigned int nbr_points = 0;
			std::vector<Coordinates> points;
			RoadDirection direction = RD_BOTH_DIRECTIONS;
			Coordinates point = {0,0};

			while(getline(is, part,',')) {
				switch (i)
				{
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
}

void ImportNodes(std::map<Coordinates, PathNode*> &nodes, std::map<unsigned int, Road*> &roads, 
	const std::map<unsigned int, ImportedRoad*> &importedRoads) {
	std::string line;
	char str[20] = "";

	strcat(str,PATH_FILE);
	strcat(str, NODES_FILE);
	std::ifstream file (str);

	if(file.is_open()) {
		
		while(getline(file, line)) {
			std::istringstream is (line);
			std::string part;
			unsigned int i = 0;

			Coordinates point = {0,0};
			Neighbor neighbor;
			unsigned int id = 0;
			unsigned int nbr_neighbor = 0;
			unsigned int id_neighbor = 0;
			unsigned int id_road = 0;

			Road* road = new Road();
			
			while(getline(is, part,',')) {
				switch (i)
				{
					case 0:
						id = ParseUint(part);
						break;
					case 1:
						point.longitude = ParseDouble(part);
						break;
					case 2:
						point.latitude = ParseDouble(part);
						break;
					case 3:
						nbr_neighbor = ParseUint(part);
						break;
					default:
						if(!i%2) {
							id_road = 0;
							id_neighbor = ParseUint(part);
						}
						else {
							id_road = ParseUint(part);
							std::map<unsigned int, ImportedRoad*>::const_iterator itRoad = importedRoads.find(id_road);
							road->distance = itRoad->second->distance;
							road->points = itRoad->second->points;
							//TODO point1 && point2
							road->direction = itRoad->second->direction;
							roads.insert(std::pair<unsigned int,Road*>(id_road,road));
						}
				}
				i++;
			}
		}
	}
}

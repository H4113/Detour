#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>

#include "import.h"
#include "utils.h"

void ImportRoads() {
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
			
			ImportedRoad road;

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
				
				road.distance = distance;
				road.points = points;
				road.point1 = id_start;
				road.point2 = id_end;
				i++;
			}
		}
	}
}

void ParseRoadLine(std::string line) {

}

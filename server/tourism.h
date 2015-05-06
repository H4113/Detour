#ifndef TOURISM_H
#define TOURISM_H

#include <vector>
#include <string>

#include "general.h"
#include "pathfinder.h"

class Database;

struct TouristicFilter
{
	bool patrimony;
	bool gastronomy;
	bool accomodation;
};

struct TouristicPlace
{
	std::string type;
	std::string typeDetail;
	std::string name;
	std::string address;
	std::string workingHours;
	Coordinates location;
};

bool BuildTouristicPath(const Path &resultPath, const std::vector<Coordinates> &initialPath, std::vector<Coordinates> &finalPath, std::vector<TouristicPlace> &places, const TouristicFilter &filter, int duration, Database *db);

/*
class Tourism
{
	public:
		Tourism(const std::vector<Coordinates> &initialPath);
		virtual ~Tourism();



	protected:
		const std::vector<Coordinates> &initialPath;
};
*/

#endif


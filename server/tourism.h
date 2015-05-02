#ifndef TOURISM_H
#define TOURISM_H

#include <vector>
#include <string>

#include "general.h"

struct TouristicPlace
{
	std::string type;
	std::string typeDetail;
	std::string name;
	std::string address;
	std::string workingHours;
	Coordinates location;
};
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


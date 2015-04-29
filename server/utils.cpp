#include <iostream>
#include <cstdlib>
#include <sstream>

#include "utils.h"
#include "general.h"
#include "pathfinder.h"

bool operator<(const Coordinates &a, const Coordinates &b)
{
	if(a.longitude < b.longitude)
		return true;
	return a.latitude < b.latitude;
}

void error(const char *msg)
{
	std::cerr << msg << std::endl;
	exit(1);
}

double ParseDouble(const std::string &s)
{
	std::istringstream iss(s);
	double ret;
	iss >> ret;
	return ret;
}

unsigned int ParseUint(const std::string &s)
{
	std::istringstream iss(s);
	unsigned int ret;
	iss >> ret;
	return ret;
}



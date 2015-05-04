#include <iostream>
#include <cstdlib>
#include <sstream>
#include <cmath>

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

double squareDist2(const Coordinates &a, const Coordinates &b)
{
	const double FACTOR = 1000;
	double m = FACTOR * a.longitude - FACTOR * b.longitude;
	double n = FACTOR * a.latitude - FACTOR * b.latitude;
	return m*m + n*n;
	//return DistanceSlow(a, b);
}

double DistanceSlow(const Coordinates &a, const Coordinates &b)
{
	const double FACTOR = 6500;
	
	Coordinates radA = {a.longitude * M_PI / 180., a.latitude * M_PI / 180.};
	Coordinates radB = {b.longitude * M_PI / 180., b.longitude * M_PI / 180.};

	double cT1 = cos(radA.longitude);
	double sT1 = sin(radA.longitude);

	double cT2 = cos(radB.longitude);
	double sT2 = sin(radB.longitude);

	return FACTOR * acos(-sT1 * sT2 * cos(radA.latitude - radB.latitude) + cT1 * cT2);
}


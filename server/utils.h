#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <map>

#include "general.h"

bool operator<(const Coordinates &a, const Coordinates &b);

void error(const char* msg);

double ParseDouble(const std::string &s);
unsigned int ParseUint(const std::string &s);

PathNode *GetClosestNode(const std::map<Coordinates, PathNode*> &nodes, const Coordinates &coord);

#endif

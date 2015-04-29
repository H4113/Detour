#ifndef UTILITIES_H
#define UTILITIES_H

#include <map>

#include "general.h"

bool operator<(const Coordinates &a, const Coordinates &b);

void error(const char* msg);

PathNode *GetClosestNode(const std::map<Coordinates, PathNode*> &nodes, const Coordinates &coord);

#endif

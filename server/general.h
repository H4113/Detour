#ifndef GENERAL_H
#define GENERAL_H

struct Coordinates
{
	double longitude;
	double latitude;
};

struct PathNode
{
	Coordinates* coordinates;
	PathNode* next;
};

#endif // GENERAL_H

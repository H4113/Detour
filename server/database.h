#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <pqxx/pqxx>

#include "general.h"

struct QTouristicLocationsOptions
{
	const Coordinates &pointA;
	const Coordinates &pointB;

	bool patrimony;
	bool gastronomy;
	bool accomodation;
};

struct TouristicPlace
{
	char *type;
	char *typeDetail;
	char *name;
	char *address;
	char *workingHours;
	Coordinates location;
};

class Database
{
	public:
		Database();
		virtual ~Database();

		bool Connect(void);

		// pointA and pointB must define a rectangle such as A is the upper-left point and B the bottom-right point.
		// IMPORTANT: works only in Lyon
		bool QueryTouristicLocations(const QTouristicLocationsOptions &options, std::vector<TouristicPlace> &places);

	protected:
		pqxx::connection *connection;
		bool connected;

};

void DB_TestDatabase(void);
int testSQLConnection();

#endif //DATABASE_H


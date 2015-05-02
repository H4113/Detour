#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <string>
#include <pqxx/pqxx>

#include "general.h"
#include "tourism.h"

struct TouristicFilter
{
	bool patrimony;
	bool gastronomy;
	bool accomodation;
};

struct QTouristicLocationsOptions
{
	double minLongitude;
	double maxLongitude;
	double minLatitude;
	double maxLatitude;

	TouristicFilter filter;
};

class Database
{
	public:
		virtual ~Database();

		static bool Connect(void);
		static bool QueryTouristicLocations(const QTouristicLocationsOptions &options, std::vector<TouristicPlace> &places);

	protected:
		Database();
		
		static pqxx::connection *connection;
		static bool connected;

};

void DB_TestDatabase(void);
int testSQLConnection();

#endif //DATABASE_H


#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <string>
#include <pqxx/pqxx>

#include "general.h"

struct QTouristicLocationsOptions
{
	double minLongitude;
	double maxLongitude;
	double minLatitude;
	double maxLatitude;

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

class Database
{
	public:
		Database();
		virtual ~Database();

		bool Connect(void);

		bool QueryTouristicLocations(const QTouristicLocationsOptions &options, std::vector<TouristicPlace> &places);

	protected:
		pqxx::connection *connection;
		bool connected;

};

void DB_TestDatabase(void);
int testSQLConnection();

#endif //DATABASE_H


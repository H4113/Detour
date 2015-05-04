#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <string>
#include <pqxx/pqxx>
#include <pthread.h>

#include "general.h"
#include "tourism.h"

struct QTouristicLocationsOptions
{
	double minLongitude;
	double maxLongitude;
	double minLatitude;
	double maxLatitude;

	struct TouristicFilter filter;
};

struct QuerySQL
{
	pthread_mutex_t *mutex;
	QTouristicLocationsOptions options;
	std::vector<TouristicPlace> *places;
};

class Database
{
	public:
		Database();		
		virtual ~Database();

		void Run(void);
		void AddQuery(const QuerySQL &query);
		static void* Routine(void* db);

	protected:
		bool QueryTouristicLocations(const QTouristicLocationsOptions &options, std::vector<TouristicPlace> &places);
		bool Connect(void);

		pqxx::connection *connection;
		bool connected;
		std::vector<QuerySQL> fifo;
		pthread_mutex_t mutex;
};

void DB_TestDatabase(void);
int testSQLConnection();

#endif //DATABASE_H


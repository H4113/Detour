#include <iostream>
#include <sstream>

#include "database.h"

Database::Database():
	connection(0),
	connected(false)
{
}

Database::~Database()
{
	delete connection;
}

bool Database::Connect(void)
{
	const char *CONNECTION_STRING = "dbname=interlyon user=postgres password=password hostaddr=127.0.0.1 port=5432";
	
	if(connected)
		return true;
	try
	{
		connection = new pqxx::connection(CONNECTION_STRING);
		connected = true;

		std::cout << "Successfully connected to " << connection->dbname() << "!" << std::endl;
	}
	catch(const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}

	return connected;
}

bool Database::QueryTouristicLocations(const QTouristicLocationsOptions &options, std::vector<TouristicPlace> &places)
{
	if(connected)
	{
		bool ret = false;
		std::ostringstream oss;
		std::ostringstream detail;
		bool hasDetail = false;
		
		oss << "SELECT typ, typ_detail, nom, adresse, ouverture, longitude, latitude FROM tourism WHERE ";
		oss << "longitude >= " << options.minLongitude << " AND longitude <= " << options.maxLongitude;
		oss << " AND latitude >= " << options.minLatitude << " AND latitude <= " << options.maxLatitude;

		if(options.patrimony)
		{
			detail << "typ='PATRIMOINE_CULTUREL' OR typ='PATRIMOINE_NATUREL' ";
			hasDetail = true;
		}
		if(options.gastronomy)
		{
			if(hasDetail)
				detail << "OR ";
			detail << "typ='RESTAURATION' OR typ='DEGUSTATION' ";
			hasDetail = true;
		}
		if(options.accomodation)
		{
			if(hasDetail)
				detail << "OR ";
			detail << "typ='HEBERGEMENT_COLLECTIF' OR typ='HEBERGEMENT_LOCATIF' OR typ='HOTELLERIE' ";
			hasDetail = true;
		}
		if(hasDetail)
			oss << " AND (" << detail.str() << ")";

		oss << ";";

		pqxx::work query(*connection);

		std::cout << oss.str() << std::endl;
		
		try
		{
			pqxx::result r = query.exec(oss.str());
			std::cout << "The query returned " << r.size() << " results" << std::endl;
			
			ret = true;
		}
		catch(const std::exception &e)
		{
			std::cerr << e.what() << std::endl;
		}

		return ret;
	}
	return false;
}

void DB_TestDatabase(void)
{
	/*
	Tourism table:
	    min(longitude) = 45.571755
		max(longitude) = 45.900444
		min(latitude) = 4.64653
		max(latitude) = 5.086112
	*/
	
	Database db;
	if(db.Connect())
	{
		QTouristicLocationsOptions options =
		{
			45.62,
			45.83,
			4.65,
			4.92,

			true,
			false,
			false
		};

		std::vector<TouristicPlace> places;

		db.QueryTouristicLocations(options, places);
	}
	else
	{
		std::cerr << "Cannot connect to database" << std::endl;
	}
}

int testSQLConnection() {
	try {
		pqxx::connection conn("dbname=interlyon user=postgres password=password hostaddr=127.0.0.1 port=5432");
		if (conn.is_open()) {
			std::cout << "We are connected to " << conn.dbname() << std::endl;

			/* Create a transactional object. */
			pqxx::work w(conn);
			pqxx::result r = w.exec("SELECT nom FROM tourism WHERE typ = 'PATRIMOINE_CULTUREL' AND typ_detail LIKE '%Musée%'");
			w.commit();
			
			for (unsigned int rownum=0; rownum < r.size(); ++rownum)
 			{
				const pqxx::result::tuple row = r[rownum];

				for (unsigned int colnum=0; colnum < row.size(); ++colnum)
				{
					const pqxx::result::field field = row[colnum];

					std::cout << field.c_str() << '\t';
				}

				std::cout << std::endl;
			}
			
			conn.disconnect ();
		}
		else {
			std::cout << "We are not connected!" << std::endl;
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}

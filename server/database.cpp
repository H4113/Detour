#include <iostream>
#include <sstream>
#include <pqxx/pqxx>

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

/*
struct TouristicPlace
{
	char *type;
	char *typeDetail;
	char *name;
	char *address;
	char *workingHours;
	Coordinates location;
};
*/

bool Database::QueryTouristicLocations(const Coordinates &pointA, const Coordinates &pointB, std::vector<TouristicPlace> &places)
{
	if(connected)
	{
		bool ret = false;
		std::ostringstream oss;
		oss << "SELECT typ, typ_detail, nom, adresse, ouverture, longitude, latitude FROM tourism WHERE ";
		oss << "longitude <= " << pointA.longitude << " AND longitude >= " << pointB.longitude;
		oss << " AND latitude >= " << pointA.latitude << " AND latitude <= " << pointB.latitude;
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
		const Coordinates POINTA = {45.83, 4.65};
		const Coordinates POINTB = {45.62, 4.92};

		std::vector<TouristicPlace> places;

		db.QueryTouristicLocations(POINTA, POINTB, places);
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
			if(false){
			const char *sql = "DROP TABLE tourism;";
			std::cout << "We are connected to " << conn.dbname() << std::endl;

			/* Create a transactional object. */
			pqxx::work drop(conn);

			try {
				drop.exec(sql);
				drop.commit();
				std::cout << "Table dropped successfully" << std::endl;
			}
			catch (const std::exception &e)
			{
				drop.abort();
				std::cout << "Table not dropped" << std::endl;
			}
			
			pqxx::work create(conn);

			/* Create SQL statement */
			sql ="CREATE TABLE tourism ("\
				"id integer CONSTRAINT pkTourism PRIMARY KEY,"\
				"typ varchar(32) not null,"\
				"typ_detail varchar(1024) not null,"\
				"nom varchar(1024) not null,"\
				"adresse varchar(1024) not null,"\
				"codepostal varchar(16) not null,"\
				"commune varchar(32) not null,"\
				"ouverture varchar(1024) not null,"\
				"longitude double precision,"\
				"latitude double precision"\
				");";

			/* Execute SQL query */
			create.exec(sql);
			create.commit();
			std::cout << "Table created successfully" << std::endl;
			}

			pqxx::work select(conn);
			pqxx::result r = select.exec("select * from tourism;");
			select.commit();
			
			//Shows nothing --> table is empty
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

#include <iostream>
#include <pqxx/pqxx>

#include "database.h"

int testSQLConnection() {
	try {
		pqxx::connection conn("dbname=interlyon user=postgres password=password hostaddr=127.0.0.1 port=5432");
		if (conn.is_open()) {
			std::cout << "We are connected to " << conn.dbname() << std::endl;

			/* Create a transactional object. */
			pqxx::work drop(conn);

			char *sql = "DROP TABLE tourism;";
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


			pqxx::work select(conn);
			pqxx::result r = select.exec("select * from tourism;");
			select.commit();
			
			//Shows nothing --> table is empty
			for (int rownum=0; rownum < r.size(); ++rownum)
 			{
				const pqxx::result::tuple row = r[rownum];

				for (int colnum=0; colnum < row.size(); ++colnum)
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

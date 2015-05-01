#include <iostream>

#include "database.h"

int testSQLConnection() {
	try {
		pqxx::connection conn("dbname=interlyon user=postgres password=password hostaddr=127.0.0.1 port=5432");
		if (conn.is_open()) {
			std::cout << "We are connected to " << conn.dbname() << std::endl;

			/* Create a transactional object. */
			pqxx::work w(conn);
			pqxx::result r = w.exec("SELECT nom FROM tourism WHERE typ = 'PATRIMOINE_CULTUREL' AND typ_detail LIKE '%Musée%'");
			w.commit();
			
			displayResult(r);
			
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

void displayResult(pqxx::result &r) {

	std::cout << "*************************" << std::endl;
	unsigned int rownum, colnum;
	for (rownum = 0; rownum < r.size(); ++rownum)
	{
		const pqxx::result::tuple row = r[rownum];
		for (colnum = 0; colnum < row.size(); ++colnum)
		{
			const pqxx::result::field field = row[colnum];
			std::cout << field.c_str() << '\t';
		}
		std::cout << std::endl;
	}
	std::cout << "*************************" << std::endl;
}

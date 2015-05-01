#ifndef DATABASE_H
#define DATABASE_H

#include <pqxx/pqxx>

int testSQLConnection();
void displayResult(pqxx::result &r);

#endif //DATABASE_H

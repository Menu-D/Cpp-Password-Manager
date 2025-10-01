/*

ignore this file ignore this file ignore this file ignore this file
ignore this file ignore this file ignore this file ignore this file
ignore this file ignore this file ignore this file ignore this file
ignore this file ignore this file ignore this file ignore this file
ignore this file ignore this file ignore this file ignore this file
ignore this file ignore this file ignore this file ignore this file
ignore this file ignore this file ignore this file ignore this file
ignore this file ignore this file ignore this file ignore this file
ignore this file ignore this file ignore this file ignore this file
ignore this file ignore this file ignore this file ignore this file
ignore this file ignore this file ignore this file ignore this file
ignore this file ignore this file ignore this file ignore this file
ignore this file ignore this file
*/


#pragma once
#include <winsqlite/winsqlite3.h>
#include "log.h"


class Database
{
private:

	sqlite3* db; 

public:

	Database();
	~Database();

	// This opens a connection to the database, it takes a const* char representing the database file path and tries to make a connection with the database.
	bool open(const char* filename); 
	// Declare the public method named close. This method returns void. 
	void close(); 

	// This declares a public method to insert a account into the database. 
	bool insertAccount(const CryptoHandler::Account& acc); 
	// This declares a public method to retrieve multiple methods from a the existing database. 
	bool fetchAccounts(std::vector<CryptoHandler::Account>& accounts); 
};


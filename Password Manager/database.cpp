#include "database.h"
#include <iostream> 

// This defines the constructor. (It intializes the private db pointer to a nullptr making sure that it does not hold any junk value right before the connection is opened. 
Database::Database() : db(nullptr) {} 
// This defines the destructor. (This calls the close() method to make sure that the database connection "db" is still open at the time the object is destroyed. 
Database::~Database() { close(); }

// This opens the DB and creates the  accounts table 
bool Database::open(const char* filename)
{
	// This opens the DB file and checks for failure.
	if (sqlite3_open(filename, &db) != SQLITE_OK)
	{
		// This prints error and return. 
		std::cerr << "Cant open DB: " << sqlite3_errmsg(db) << "\n"; 
		return false; 
	}

	// This makes SQL create the "accounts" table if it does not exist. 
	const char* create_sql =
		"CREATE TABLE IF NOT EXISTS accounts ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT,"
		"salt BLOB NOT NULL,"
		"nonce_user BLOB NOT NULL,"
		"nonce_pass BLOB NOT NULL,"
		"ciphertext_user BLOB NOT NULL,"
		"ciphertext_pass BLOB NOT NULL"
		");";

	// Execute the table creation for SQL. Checks for failure. 
	if (sqlite3_exec(db, create_sql, nullptr, nullptr, nullptr) != SQLITE_OK) 
	{
		std::cerr << "Failed to create table: " << sqlite3_errmsg(db) << "\n";
		return false;
	}

	// Success 
	return true; 
}

// This close the database connection 
void Database::close() {
	// This checks if the connection is open
	if (db) {
		// This closes the connection
		sqlite3_close(db);
		// This sets a pointer to null
		db = nullptr;
	}
}

// Inserts a new account record
bool Database::insertAccount(const CryptoHandler::Account& acc) {
	// This prepares statement with placeholders
	const char* insert_sql =
		"INSERT INTO accounts (salt, nonce_user, nonce_pass, ciphertext_user, ciphertext_pass) "
		"VALUES (?, ?, ?, ?, ?);";

	sqlite3_stmt* stmt;
	// This prepares the statements and check for failure
	if (sqlite3_prepare_v2(db, insert_sql, -1, &stmt, nullptr) != SQLITE_OK) {
		std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
		return false;
	}

	// Bind salt (parameter 1)
	sqlite3_bind_blob(stmt, 1, acc.salt, crypto_pwhash_SALTBYTES, SQLITE_STATIC);
	// Bind user nonce (parameter 2)
	sqlite3_bind_blob(stmt, 2, acc.nonce_user, crypto_secretbox_NONCEBYTES, SQLITE_STATIC);
	// Bind pass nonce (parameter 3)
	sqlite3_bind_blob(stmt, 3, acc.nonce_pass, crypto_secretbox_NONCEBYTES, SQLITE_STATIC);
	// Bind user ciphertext vector data (parameter 4)
	sqlite3_bind_blob(stmt, 4, acc.ciphertext_user.data(), acc.ciphertext_user.size(), SQLITE_STATIC);
	// Bind pass ciphertext vector data (parameter 5)
	sqlite3_bind_blob(stmt, 5, acc.ciphertext_pass.data(), acc.ciphertext_pass.size(), SQLITE_STATIC);

	// This executes the statement
	bool success = (sqlite3_step(stmt) == SQLITE_DONE);
	// This cleans up statement resources
	sqlite3_finalize(stmt);
	return success;

}

// This fetches all account records into the vector
bool Database::fetchAccounts(std::vector<CryptoHandler::Account>& accounts) {
	const char* query_sql = "SELECT salt, nonce_user, nonce_pass, ciphertext_user, ciphertext_pass FROM accounts;";
	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(db, query_sql, -1, &stmt, nullptr) != SQLITE_OK) {
		std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
		return false;
	}
	// Loop through all resulting rows
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		CryptoHandler::Account acc;
		// This fetches and copy salt (Col 0)
		memcpy(acc.salt, sqlite3_column_blob(stmt, 0), crypto_pwhash_SALTBYTES);
		// This fetches and copy user nonce (Col 1)
		memcpy(acc.nonce_user, sqlite3_column_blob(stmt, 1), crypto_secretbox_NONCEBYTES);
		// This fetches and copy pass nonce (Col 2)
		memcpy(acc.nonce_pass, sqlite3_column_blob(stmt, 2), crypto_secretbox_NONCEBYTES);
		// This fetches user ciphertext blob data and size (Col 3)
		const void* user_blob = sqlite3_column_blob(stmt, 3);
		int user_size = sqlite3_column_bytes(stmt, 3);
		// This assigns data to std::vector
		acc.ciphertext_user.assign((const unsigned char*)user_blob, (const unsigned char*)user_blob + user_size);
		// This fetches pass ciphertext blob data and size (Col 4)
		const void* pass_blob = sqlite3_column_blob(stmt, 4);
		int pass_size = sqlite3_column_bytes(stmt, 4);
		// This assign data to std::vector
		acc.ciphertext_pass.assign((const unsigned char*)pass_blob, (const unsigned char*)pass_blob + pass_size);
		// This moves the new account into the vector
		accounts.push_back(std::move(acc));
	}
	// This cleans up statement resources
	sqlite3_finalize(stmt);
	return true;
}

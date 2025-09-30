#include <iostream> 
#include "cui.h"
#include "database.h"
#include <winsqlite/winsqlite3.h>


int main()
{
	Database db; 
	if (!db.open("vault.db"))
	{
		std::cerr << "failed to open the database.\n"; 
		return 1; 
	}

	Vault vault(&db); 
	RunProgram runner(); 
	runner.runProgram(); 
	return 0; 
}
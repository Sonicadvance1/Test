#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <sqlite3.h>
#include "Database.hpp"

// Just a small function
bool sqlerr(int x)
{
	if(x != SQLITE_OK) 
	{
		printf("SQL failed to open Database with error: %d\n", x);
		return false;
	}
	return true;
}

namespace Database
{
	// Our Local SQLite DB handle
	sqlite3 *_Db;
	
	bool Init()
	{
		// These three lines make sure we are using the correct SQLite library with what is compiled in.
		assert( sqlite3_libversion_number()==SQLITE_VERSION_NUMBER );
		assert( strcmp(sqlite3_sourceid(),SQLITE_SOURCE_ID)==0 );
		assert( strcmp(sqlite3_libversion(),SQLITE_VERSION)==0 );
		if(!sqlite3_threadsafe())
			printf("Warning SQLite3 is NOT compiled thread-safe! Report this issue!\n");
		// Try Opening read only first
		int err = sqlite3_open_v2(DATABASE_FILENAME, &_Db, SQLITE_OPEN_READONLY, NULL);
		if(err == SQLITE_CANTOPEN) // Doesn't exist
		{
			err = 0; // Reset error
			err = sqlite3_open(DATABASE_FILENAME, &_Db);
			if(!sqlerr(err))
				return false;
			const char PlayerTable[] = "CREATE TABLE "PLAYER_TABLE" (ID INTEGER PRIMARY KEY, PlayerName TEXT);";
			Exec(PlayerTable);
		}
		if(!sqlerr(err))
			return false;
		return true;
	}
	void Shutdown()
	{
		int err = sqlite3_close(_Db);
		if(err != SQLITE_OK)
			printf("SQLite3 didn't return okay on close! Report this issue!\n");
	}
	bool Exec(const char* Command)
	{
		char *Msg;
		int err = sqlite3_exec(_Db, Command, NULL, NULL, &Msg);
		if(!sqlerr(err))
		{
			printf("Additional Data: %s\n", Msg);
			sqlite3_free(Msg);
			return false;
		}
		return true;
	}
	
	bool Table(const char* Command, char ***Result, int *Rows, int *Col)
	{
		char *Msg;
		int err = sqlite3_get_table(_Db, Command, Result, Rows, Col, &Msg);
		if(!sqlerr(err))
		{
			printf("Additional Data: %s\n", Msg);
			sqlite3_free(Msg);
			return false;
		}
		(*Rows)++; // Rows doesn't include the table names, add one to include it
		return true;
	}
	
	void FreeTable(char **Result)
	{
		sqlite3_free_table(Result);
	}
}

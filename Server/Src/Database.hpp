
#define DATABASE_FILENAME "Database.sqlite3"
#define PLAYER_TABLE "Players"
/*
	// Shows how to use the Database namespace
	// Just a really simple example
int Rows, Cols;
char **Result;
if(Database::Table("SELECT * FROM "PLAYER_TABLE";", &Result, &Rows, &Cols))
{
	printf("%d:%d\n", Rows, Cols);
	for(int a = 0; a < Rows; ++a)
		for(int b = 0; b < Cols; ++b)
			printf("%s\n", Result[a * Cols + b]);
}
Database::FreeTable(Result);
*/


namespace Database
{
	bool Init();
	void Shutdown();
	
	// Execute a SQL command
	bool Exec(const char* Command);
	
	// Executes the SQL command and
	// Returns a table in a two dimensional char array
	// Returns Result in Result
	// Rows and Col returns how many rows and columns were returned
	// *First Row of results is the table names*
	bool Table(const char* Command, char ***Result, int *Rows, int *Col);
	// Frees a table received from Table() above
	void FreeTable(char **Result);
}

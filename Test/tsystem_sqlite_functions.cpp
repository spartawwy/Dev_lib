#include <TLib/core/tsystem_sqlite_functions.h>

using namespace TSystem;
 
// load all table names from sqlite database
std::set<std::string> utility::LoadTableNames(SQLite::SQLiteConnection& sqlite_db, const std::string& db_name)
{
	// todo:
	std::set<std::string> table_names;
	return table_names;
}

// check if exist table 
bool utility::ExistTable(const std::string& name, SQLite::SQLiteConnection& sqlite_db, const std::string& db_name)
{
	// todo:
	return true;
}
 
#include "SQLite/sqlite_connection.h"

#include "TLib/core/tsystem_utility_functions.h"

using namespace SQLite;
using namespace TSystem;

SQLiteConnection::SQLiteConnection()
    : connection_(nullptr)
{
    // todo:
}

SQLiteConnection::~SQLiteConnection()
{
    // todo:
    Close();
}

// set busy timeout
void SQLiteConnection::SetBusyTimeout(int ms) const
{
    // todo:
    if( connection_ )
        sqlite3_busy_timeout(connection_, ms);
}

// open main database connection
SQLiteCode SQLiteConnection::Open(const char* full_me
	, OpenMode om
	, ThreadMode tm
	, CacheMode cm)
{
    // ndchk:
    int flag = 0;
    switch(om)
    {
    case OpenMode::READ: flag |= SQLITE_OPEN_READONLY; break;
    case OpenMode::READ_WRITE: flag |= SQLITE_OPEN_READWRITE; break;
    case OpenMode::READ_WRITE_CREATE: flag |= SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE; break;
    default: break;
    }
    switch(tm)
    {
    case ThreadMode::SERIAL: flag |= SQLITE_OPEN_FULLMUTEX; break;
    case ThreadMode::THREAD: flag |= SQLITE_OPEN_NOMUTEX; break;
    default: break;
    }
    switch (cm)
    {
        case CacheMode::SHARED: flag |= SQLITE_OPEN_SHAREDCACHE; break;
        case CacheMode::PRIVATE: flag |= SQLITE_OPEN_PRIVATECACHE; break;
        default: break;
    }   

    int result = sqlite3_open_v2(full_me, &connection_, flag, nullptr);
    if( result != SQLITE_OK )
    {
        return static_cast<SQLiteCode>(result);
    }
         
    return SQLiteCode::OK;
}
		
// close database connection
void SQLite::SQLiteConnection::Close()
{
    // ndchk
    if( connection_ )
        sqlite3_close_v2(connection_);
    connection_ = nullptr;
}

// fast execute sql, for repeated insert / update
// use SQLiteStatement class
// throw SQLiteException when fail
// return true if ok and false if abort by handler
bool SQLiteConnection::ExecuteSQL(const char* sql
	, QueryReturnHandlerType&& handler) const
{
    if( !connection_ )
        return false;

     static auto call_back = [](void *param, int f_num, char **f_value, char **f_name) ->int
        {
            //printf("%s %s %s\n", __FUNCTION__, *f_value, *f_name);
            (*(QueryReturnHandlerType*)param)(f_num, f_value, f_name);
            return 0;
        };
    char *error_msg;
    int ret = sqlite3_exec(connection_, sql, call_back, &handler/* 1st argument to callback */, &error_msg);
    return ret == SQLITE_OK;
}

		
// number of record changed
int SQLiteConnection::NumChanged() const
{
    return sqlite3_changes(connection_);
}

// attach / detach additional database
void SQLite::AttachDatabase( const std::string& full_file_name, const std::string& db_name, const SQLiteConnection& conn )
{
    char *zErrMsg = nullptr;
    sqlite3_exec(conn.connection(), utility::FormatStr("ATTACH '%s' AS '%s'", full_file_name.c_str(), db_name.c_str()).c_str(), nullptr, nullptr, &zErrMsg);
}

void SQLite::DetachDatabase( const std::string& db_name, const SQLiteConnection& conn )
{
    char *zErrMsg = nullptr;
    sqlite3_exec(conn.connection(), utility::FormatStr("DETACH '%s' ", db_name.c_str()).c_str(), nullptr, nullptr, &zErrMsg);
}

// begin / end transaction
void SQLite::BeginTransaction( const SQLiteConnection& conn )
{
    // todo:
}

void SQLite::EndTransaction( const SQLiteConnection& conn )
{
    // todo:
}
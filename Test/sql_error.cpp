#include "SQLite/sqlite_error.h"

//using namespace SQLite;

const char* SQLite::SQLiteCodeStr(SQLiteCode code)
{
    static std::string str;
    switch(code)
    {
    default : return "";
    }
    return str.c_str();
}

// 
SQLite::SQLiteException::SQLiteException(SQLiteCode code) 
    : code_(code)
    , other_info_(SQLiteCodeStr(code))
{
    // 
}

const char* SQLite::SQLiteException::what() const
{
    // todo:
    return other_info_.c_str();
}
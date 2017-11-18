#include "SQLite/sqlite_statement.h"
#include "SQLite/Sqlite3.h"
#include "TLib/core/tsystem_error.h"
#include "TLib/core/tsystem_core_error.h"

using namespace SQLite;
using namespace TSystem;

SQLiteStatement::SQLiteStatement(const std::shared_ptr<SQLiteConnection>& conn)
    : conn_(conn)
    , stmt_(nullptr)
{
   // sqlite_int64(
}

SQLiteStatement::SQLiteStatement( SQLiteStatement&& in)
    : conn_(std::move(in.conn_))
    , stmt_(in.stmt_)
{
}

void SQLiteStatement::CompileSQL(const char* sql)
{
    auto res = sqlite3_prepare_v2(conn_->connection(), sql, strlen(sql), &stmt_, 0);
    if( res != SQLITE_OK )
    {
         ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
            , "SQLiteStatement::CompileSQL"
            , sql);
        return;
    }
}

SQLiteCode SQLiteStatement::Clear()
{
    if( stmt_ && sqlite3_finalize(stmt_) != SQLITE_OK )
    {
        ThrowTException(CoreErrorCategory::ErrorCode::BAD_CONTENT
            , "SQLiteStatement::Clear"
            , "");
    }
    return SQLiteCode::OK;
}

const char* SQLiteStatement::SQL() const
{
    // todo:
    return "";
}

bool SQLiteStatement::Evaluate() const
{
    auto ret = sqlite3_step(stmt_);
    if( ret == SQLITE_DONE || ret == SQLITE_ROW )
        return true;
    else 
        return false;
}

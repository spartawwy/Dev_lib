// test2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <TLib\core\tsystem_utility_functions.h>
#include "SQLite/sqlite_connection.h"

#pragma comment(lib, "TLib_d.lib")

using namespace TSystem;

int _tmain(int argc, _TCHAR* argv[])
{

	auto str = utility::FormatStr("hello %d", 1);

	SQLite::SQLiteConnection  conn_obj;
	auto ret = conn_obj.Open("Z:\\WZF\\data\\layout\\dev.laydb");

	return 0;
}


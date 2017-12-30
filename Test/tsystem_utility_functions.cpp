#include "TLib/core/tsystem_utility_functions.h"

#include <direct.h>

using namespace TSystem;

static std::string&   replace_all_distinct(std::string& str, const std::string& old_value, const std::string& new_value);
// create file dir
void TSystem::utility::CreateDir(const std::string& dir)
{ 
    /*if( dir.find(".") != std::string::npos )
    {
         printf("CreateDir %s is illegal!", dir.c_str());
         return;
    }*/
    char old_dir[1024] = {0};
    _getcwd(old_dir, sizeof(old_dir));

    std::string dir_copy = dir;
	replace_all_distinct(dir_copy, "\\", "//");
    auto folders = utility::split(dir_copy, "//");
    if( folders.size() < 2 )
    {
        printf("CreateDir %s fail!", dir.c_str());
        _chdir(old_dir);
        return;
    }
    for( unsigned i = 1; i < folders.size(); ++i )
    {
        _chdir(folders.at(i-1).c_str());
        _mkdir(folders.at(i).c_str());
    }
    _chdir(old_dir);
}

std::string utility::host()
{
	// todo: 
	return "WIN20141227ZVZ";
}

std::string&   replace_all_distinct(std::string& str, const std::string& old_value, const std::string& new_value)   
{   
    for(std::string::size_type pos(0); pos != std::string::npos; pos += new_value.length())   
    {   
        if(   (pos=str.find(old_value,pos)) != std::string::npos   )   
            str.replace(pos, old_value.length(), new_value);   
        else   break;   
    }   
    return   str;   
}   
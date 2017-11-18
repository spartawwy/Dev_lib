#include <TLib/core/tsystem_app_config.h>

using namespace TSystem;

AppConfiguration::AppConfiguration()
	: cfg_dirs_()
	, dict_()
	, params_(dict_)
	, last_update_()
{

}

AppConfiguration::~AppConfiguration()
{

}

void AppConfiguration::AddDir(const std::string& path)
{
	// todo:
}

bool AppConfiguration::Load(const std::string& name)
{
	// todo:
	return true;
}

// read / test default value
bool  AppConfiguration::GetVal(const std::string& name, int* res) const 
{
	// todo:
	return true;
}

bool  AppConfiguration::GetVal(const std::string& name, double* res) const 
{
	// todo:
	return true;
} 
bool  AppConfiguration::GetVal(const std::string& name, std::string* res) const  
{
	// todo:
	return true;
} 

void  AppConfiguration::GetVal(const std::string& name, int& res) const 
{
	// todo: 
} 

void  AppConfiguration::GetVal(const std::string& name, double& res) const 
{
	// todo: 
} 

void  AppConfiguration::GetVal(const std::string& name, std::string& res) const 
{
    
	// todo:
} 

// read / test value for named tag
bool  AppConfiguration::GetVal(const std::string& name, const std::string& group, int* res) const 
{
	// todo:
	return true;
} 
bool  AppConfiguration::   GetVal(const std::string& name, const std::string& group, double* res) const 
{
	// todo:
	return true;
} 
bool  AppConfiguration::GetVal(const std::string& name, const std::string& group, std::string* res) const 
{
	// todo:
	return true;
} 

// versions throw exception
void  AppConfiguration::GetVal(const std::string& name, const std::string& group, int& res) const 
{
	// todo:
	return;
} 

void  AppConfiguration::GetVal(const std::string& name, const std::string& group, double& res) const 
{
	// todo: 
} 
void  AppConfiguration::GetVal(const std::string& name, const std::string& group, std::string& res) const 
{
	// todo: 
} 

void TSystem::LoadAppConfig(AppInfo& info, AppConfiguration& config)
{
	// todo:
}
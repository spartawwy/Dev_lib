#include <TLib/core/tsystem_appbase.h>
#include <TLib/core/tsystem_core_paths.h>

using namespace TSystem;

// app information 
AppInfo::AppInfo(const std::string& type, const std::string& name, const std::string& version)
	: type_(type), name_(name), version_(version), log_dir_(utility::RootLogDir()), node_(), pid_(0), port_(0)
{
	// todo:
}
	 
 AppBase::~AppBase()
 {
 }

// shutdown all component, should not throw any exception
void AppBase::Shutdown()
{
    local_logger_.Shutdown();
    task_pool_.Shutdown();
}

// wait all task in task_pool_ finish
void AppBase::WaitShutdown()
{
    task_pool_.Join();
    shutdown_ = true;
}

// stop all task, stop immediately.
void AppBase::FireShutdown()
{
    task_pool_.Shutdown();
}
   
AppBase::AppBase(const std::string& type, const std::string& name, const std::string& version)
	: AppInfo(type, name, version), local_logger_(task_pool_), shutdown_sig_(task_pool_)
{
    shutdown_ = false;
    local_logger_.SetName(name);
    local_logger_.SetDir(utility::RootDir());
    local_logger_.StartFlush();
}

void AppBase::Initiate()
{
	// todo:
}
	 
 

//---------------------------------------------------------
// free utility function for AppInfo and AppBase
//---------------------------------------------------------

// appdir
std::string  AppDir(const AppInfo& app)
{
	return app.work_dir();
}

// print version string
std::string  BuildInfo(const AppInfo& app)
{
	return app.version();
}

std::string TSystem::AppDir(const AppInfo& app)
{
	// todo: need check
	return app.work_dir();
}

// print version string
std::string TSystem::BuildInfo(const AppInfo& app)
{
	// todo: need check
	return app.version();
}
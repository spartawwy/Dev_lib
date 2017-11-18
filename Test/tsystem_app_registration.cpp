#include <TLib/core/tsystem_app_registration.h>

#include "shared_memory.hpp"

using namespace TSystem;

AppRegistration::AppRegistration()
{
}

AppRegistration::~AppRegistration()
{
}

bool AppRegistration::CheckAndRegisterPid(unsigned short pid)
{
	// TODO: register pid in shared_memory
	return true;
}

void AppRegistration::CleanRegistration()
{
	// TODO: 
}
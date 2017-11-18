#ifndef TSYSTEM_DOMAIN_NAME_
#define TSYSTEM_DOMAIN_NAME_

#include <string>
#include <tuple>

namespace TSystem
{
namespace utility
{
	// get host full name, hostname.domainname
	// currently only implemented for windows
	// rturn (computer_name, domain_name)
	// throw TException when fail
	std::pair<std::string, std::string>	GetHostFullName();
}
}

#endif
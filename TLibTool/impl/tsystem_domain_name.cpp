#include "tsystem_domain_name.h"

#include "TLib/core/tsystem_core_error.h"
#include "TLib/core/tsystem_utility_functions.h"

#ifdef _WIN32_WINNT
#include <comdef.h>
#include <Iads.h>
#pragma comment(lib, "ADSIid.Lib")
#endif // _WIN32_WINNT

#ifdef _WIN32_WINNT
std::pair<std::string, std::string> TSystem::utility::GetHostFullName()
{
	CoInitialize(NULL);  

	IADsWinNTSystemInfo *pNTsys;
	HRESULT hr = CoCreateInstance(CLSID_WinNTSystemInfo,
								  NULL,
								  CLSCTX_INPROC_SERVER,
								  IID_IADsWinNTSystemInfo,
								  (void**)&pNTsys);
	
	if( !SUCCEEDED(hr) )
	{
		CoUninitialize();  
		TSystem::ThrowTException(TSystem::CoreErrorCategory::ErrorCode::BAD_IO,
			"GetHostFullName",
			TSystem::FormatThirdPartyError("WinAPI", GetLastError(), ""));
	}

	BSTR  bStr;
	pNTsys->get_ComputerName(&bStr);
	std::string computer_name = _com_util::ConvertBSTRToString(bStr);  
	SysFreeString(bStr);  

	pNTsys->get_DomainName(&bStr);
	std::string domain_name = _com_util::ConvertBSTRToString(bStr);  
	SysFreeString(bStr);  
	CoUninitialize();  

	return std::make_pair(std::move(computer_name), std::move(domain_name));
}
#endif // _WIN32_WINNT


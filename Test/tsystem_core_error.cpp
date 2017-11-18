#include <TLib/core/tsystem_core_error.h>
#include <TLib/core/tsystem_string_functions.h>

using namespace TSystem;

CoreErrorCategory::CoreErrorCategory()
{
	// todo:
}

CoreErrorCategory& CoreErrorCategory::GetCategory()
{
	// todo:
	static CoreErrorCategory core_error_category;
	return core_error_category;
}
 

const std::string& CoreErrorCategory::name() const
{
    static std::string  core_erro("core_error");
    return core_erro;
}

bool  CoreErrorCategory::CheckOK( short ec ) const
{
    // ndchk:
    return static_cast<ErrorCode>(ec) == ErrorCode::OK;
}

const std::string& CoreErrorCategory::Message( short ec ) const
{
    static const std::string bad_ec("not this error category");
	static const std::string ok("ok"); 
    static const std::string unknow("unknow error");
	switch (ec)
	{
	case ErrorCode::OK:
		return ok;
    default: return unknow;
    } 
}
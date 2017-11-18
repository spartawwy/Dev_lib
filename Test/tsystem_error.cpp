#include <TLib/core/tsystem_error.h>
#include <TLib/core/tsystem_utility_functions.h>

#include <TLib/core/tsystem_core_error.h>

using namespace TSystem;

const std::string& TErrorCategory::name() const 
{
	// todo:
	return "";
}

bool TErrorCategory::CheckOK( short ec ) const
{
	// todo:
	return true;
}

// error code's message
const std::string& TErrorCategory::Message( short ec ) const
{
	// todo:
    static std::string  terror_cat("TErrorCategory");
    switch(ec)
    {
    default: return terror_cat;
    }
	
}

TError::TError( short code, TErrorCategory& err_cate ) : error_code_(code), error_cate_(&err_cate)
{

}
TError::TError( TError&& in) : error_code_(in.error_code_), error_cate_(in.error_cate_), func_name_(std::move(in.func_name_)), other_info_(std::move(in.other_info_))
{

}
TError::TError() : error_code_(0), error_cate_()
{
    *this = MakeTError(CoreErrorCategory::ErrorCode::OK);
}
	
// = operator
TError& TError::operator=(TError&& in)
{
	this->error_cate_ = in.error_cate_;
	this->error_code_ = in.error_code_;
	this->other_info_ = std::move(in.other_info_);
	this->func_name_ = std::move(in.func_name_);
	return *this;
}

TError& TError::operator=(TError& in)
{
	this->error_cate_ = in.error_cate_;
	this->error_code_ = in.error_code_;
	this->other_info_ = in.other_info_;
	this->func_name_ = in.func_name_;
	return *this;
}

// format error string based on TError
std::string TSystem::ErrorString( const TError& er)
{
    /*const TErrorCategory& ctg = er.category();
    std::string chk0 = (&ctg)->Message(er.code());
    auto chk = er.category().Message(er.code());
    
    std::string msg = er.category().Message(er.code());

    std::string  ck_str0 = chk.c_str(); 
    std::string  ck_str = er.category().Message(er.code()).c_str();*/
	return utility::FormatStr( "eorro:%d %s", er.code(), er.category().Message(er.code()).c_str() );
 
}

// format third party error
std::string TSystem::FormatThirdPartyError(const char* lib_name, int ec, const char* msg)
{
	return utility::FormatStr("module %s error:%d %s", lib_name, ec, msg);
}


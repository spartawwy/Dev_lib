#include <TLib/core/tsystem_configuration.h>

using namespace TSystem;

CFDictionary::CFDictionary()
    : params_(64)
{

}

CFDictionary::~CFDictionary()
{

}

bool CFDictionary::Load(const std::string& dict_file)
{
    // todo: 
    return true;
}

bool CFDictionary::Parse(const std::string& param_str, TError& err, CfgSpec* spec) const
{
    // todo: 
    return true;
}

//////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////
ParamItem::ParamItem(const std::string& name)
    : name_()
    , regex_list_()
    , taged_map_()
{

}

ParamItem::~ParamItem()
{
}

ParamItem& ParamItem::operator=( ParamItem&& rh)
{
    if( this == &rh) 
        return *this;
    name_ = std::move(rh.name_);
    regex_list_ = std::move(rh.regex_list_);
    taged_map_ = std::move(rh.taged_map_);
    return *this;
}

void ParamItem::AddRegexGrp(const std::regex& pattern, const std::string& val)
{
    // todo: 
}

// add value for a named tag
void ParamItem::AddTagedGrp(const std::string& tag, const std::string& val)
{
}

// add default value
void ParamItem::AddDefault(const std::string& val)
{
}
		
// read / test default value
const std::string& ParamItem::Val()  const
{// todo: 
    return "";
}

// read / test value for a tag, will search named tag then regex
const std::string& ParamItem::Val(const std::string& grpname) const
{// todo: 
    return "";
}

///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
CFParams::CFParams(const CFDictionary& dict)
    : dict_(dict)
    , params_(64)
{
}

CFParams::~CFParams()
{ 
    // todo:
}

// load parameter, parse with a dictionary 
// throw TException when there is bad config line
bool CFParams::Load(const std::string& cfg_file)
{
    // todo:
    return true;
}

// read parameter 
const std::string& CFParams::GetParamStr(const std::string& name, const std::string& group) const
{
    // todo:
    return "";
}
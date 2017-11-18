#include <TLib/core/tsystem_core_paths.h>

using namespace TSystem;

const std::string& utility::ProjectTag(const char* p_tag)
{
	static std::string project_tag = p_tag;
	return project_tag;
}
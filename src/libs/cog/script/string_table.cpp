#include "string_table.hpp"
#include "utility/make_unique.hpp"

char const* gorc::cog::string_table::add_string(std::string const &str)
{
    strings.push_back(make_unique<std::string>(str));
    return strings.back()->c_str();
}

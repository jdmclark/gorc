#include "string_table.hpp"

char const* gorc::cog::string_table::add_string(std::string const &str)
{
    strings.push_back(std::make_unique<std::string>(str));
    return strings.back()->c_str();
}

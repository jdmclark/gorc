#include "verb.hpp"

gorc::cog::verb::verb(std::string const &name,
                      value_type return_type,
                      std::vector<value_type> &&argument_types)
    : name(name)
    , return_type(return_type)
    , argument_types(argument_types)
{
    return;
}

gorc::cog::verb::~verb()
{
    return;
}

#include "value_mapping.hpp"
#include "log/log.hpp"

gorc::cog_scenario_value_mapping::cog_scenario_value_mapping(cog_scenario const &scn)
    : scn(scn)
{
    return;
}

gorc::cog::value gorc::cog_scenario_value_mapping::operator()(
        cog::value_type,
        std::string const &value) const
{
    auto it = scn.resources.find(value);
    if(it == scn.resources.end()) {
        LOG_FATAL(format("no scenario resource named '%s'") % value);
    }

    return it->second;
}

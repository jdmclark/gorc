#include "source_type.hpp"
#include "log/log.hpp"
#include <unordered_map>

using gorc::cog::source_type;

namespace {
    std::unordered_map<std::string, source_type> str_map {
        { "free", source_type::free },
        { "camera", source_type::camera },
        { "actor", source_type::actor },
        { "weapon", source_type::weapon },
        { "debris", source_type::debris },
        { "item", source_type::item },
        { "explosion", source_type::explosion },
        { "cog", source_type::cog },
        { "ghost", source_type::ghost },
        { "corpse", source_type::corpse },
        { "player", source_type::player },
        { "particle", source_type::particle }
    };
}

gorc::cog::source_type gorc::cog::as_source_type(std::string const &str)
{
    auto it = str_map.find(str);
    if(it == str_map.end()) {
        LOG_FATAL(format("unknown source type '%s'") % str);
    }

    return it->second;
}

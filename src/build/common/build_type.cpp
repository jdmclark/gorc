#include "build_type.hpp"
#include "log/log.hpp"
#include <unordered_map>

namespace {

    std::unordered_map<std::string, gorc::build_type> bt_map {
        { "release", gorc::build_type::release },
        { "debug", gorc::build_type::debug },
        { "coverage", gorc::build_type::coverage }
    };

}

gorc::build_type gorc::to_build_type(std::string const &str)
{
    auto it = bt_map.find(str);
    if(it == bt_map.end()) {
        LOG_FATAL(format("Unknown build type '%s'") % str);
    }

    return it->second;
}

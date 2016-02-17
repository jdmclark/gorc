#include "external_lib_type.hpp"
#include "log/log.hpp"
#include <unordered_map>

namespace {

    std::unordered_map<std::string, gorc::external_lib_type> lib_type_str_map {
        { "boost", gorc::external_lib_type::boost },
        { "sfml", gorc::external_lib_type::sfml }
    };

}

gorc::external_lib_type gorc::to_external_lib_type(std::string const &str)
{
    auto it = lib_type_str_map.find(str);
    if(it == lib_type_str_map.end()) {
        LOG_FATAL(format("unknown external library type '%s'") % str);
    }

    return it->second;
}

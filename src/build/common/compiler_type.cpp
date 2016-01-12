#include "compiler_type.hpp"
#include "log/log.hpp"
#include <unordered_map>

namespace {

    std::unordered_map<std::string, gorc::compiler_type> bt_map {
        { "gcc", gorc::compiler_type::gcc }
    };

}

gorc::compiler_type gorc::to_compiler_type(std::string const &str)
{
    auto it = bt_map.find(str);
    if(it == bt_map.end()) {
        LOG_FATAL(format("Unknown compiler type '%s'") % str);
    }

    return it->second;
}

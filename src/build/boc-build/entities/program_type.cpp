#include "program_type.hpp"
#include "log/log.hpp"
#include <unordered_map>

namespace {

    std::unordered_map<std::string, gorc::program_type> program_type_map {
        { "release", gorc::program_type::release },
        { "test", gorc::program_type::test },
        { "build", gorc::program_type::build }
    };

}

gorc::program_type gorc::to_program_type(std::string const &str)
{
    auto it = program_type_map.find(str);
    if(it == program_type_map.end()) {
        LOG_FATAL(format("unknown program type '%s'") % str);
    }

    return it->second;
}

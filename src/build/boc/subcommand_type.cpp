#include "subcommand_type.hpp"
#include "log/log.hpp"
#include <unordered_map>
#include <sstream>

namespace {

    std::unordered_map<std::string, gorc::subcommand_type> type_map {
        { "build", gorc::subcommand_type::build },
        { "test", gorc::subcommand_type::test },
        { "clean", gorc::subcommand_type::clean },
        { "coverage-report", gorc::subcommand_type::coverage_report },
        { "coveralls-coverage-report", gorc::subcommand_type::coveralls_coverage_report }
    };

}

gorc::subcommand_type gorc::to_subcommand_type(std::string const &st)
{
    auto it = type_map.find(st);
    if(it == type_map.end()) {
        std::stringstream errmsg;
        errmsg << "Unknown command '" << st << "'. Valid commands are:" << std::endl;
        for(auto const &t : type_map) {
            errmsg << "    " << t.first << std::endl;
        }

        LOG_FATAL(errmsg.str());
    }

    return it->second;
}

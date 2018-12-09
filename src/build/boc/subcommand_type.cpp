#include "subcommand_type.hpp"
#include "log/log.hpp"
#include <map>
#include <sstream>

namespace {

    std::map<std::string, gorc::subcommand_type> type_map{
        {"test", gorc::subcommand_type::test},
        {"coverage-report", gorc::subcommand_type::coverage_report},
        {"coveralls-coverage-report", gorc::subcommand_type::coveralls_coverage_report}};
}

gorc::subcommand_type gorc::to_subcommand_type(std::string const &st)
{
    auto it = type_map.find(st);
    if(it == type_map.end()) {
        std::stringstream errmsg;
        errmsg << "Unknown command '" << st << "'. Valid commands are:";
        for(auto const &t : type_map) {
            errmsg << std::endl << "- " << t.first;
        }

        LOG_FATAL(errmsg.str());
    }

    return it->second;
}

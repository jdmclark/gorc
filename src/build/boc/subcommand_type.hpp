#pragma once

#include <string>

namespace gorc {

    enum class subcommand_type {
        build,
        test,
        clean,
        coverage_report,
        coveralls_coverage_report
    };

    subcommand_type to_subcommand_type(std::string const &);

}

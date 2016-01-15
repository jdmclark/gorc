#pragma once

#include <string>

namespace gorc {

    enum class subcommand_type {
        build,
        test,
        clean
    };

    subcommand_type to_subcommand_type(std::string const &);

}

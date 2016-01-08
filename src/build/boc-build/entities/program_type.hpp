#pragma once

#include <string>

namespace gorc {

    enum class program_type {
        release,
        test,
        build
    };

    program_type to_program_type(std::string const &);

}

#pragma once

#include <string>

namespace gorc {

    enum class compiler_type {
        gcc
    };

    compiler_type to_compiler_type(std::string const &);

}

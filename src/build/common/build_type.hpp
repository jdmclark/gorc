#pragma once

#include <string>

namespace gorc {

    enum class build_type {
        release,
        debug,
        coverage
    };

    build_type to_build_type(std::string const &);

}

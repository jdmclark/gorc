#pragma once

#include "io/path.hpp"

namespace gorc {

    class extract_path {
    public:
        path const value;

        extract_path(path const &value);
    };

}

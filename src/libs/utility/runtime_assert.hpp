#pragma once

#include <stdexcept>

namespace gorc {

    inline void runtime_assert(bool e, std::string const &msg)
    {
        if(!e) {
            throw std::runtime_error(msg);
        }
    }
}

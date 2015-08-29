#pragma once

#include <stdexcept>

namespace gorc {
    class logged_runtime_error : public std::runtime_error {
    public:
        logged_runtime_error();
    };
}

#pragma once

#include "diagnostic_context_location.hpp"
#include <cstddef>

namespace gorc {

    class diagnostic_context {
    private:
        size_t diagnostic_context_handle = 0;

    public:
        explicit diagnostic_context(diagnostic_context_location const &loc);
        diagnostic_context(maybe<char const *> filename,
                           int first_line = 0,
                           int first_col = 0,
                           int last_line = 0,
                           int last_col = 0);
        ~diagnostic_context();

        diagnostic_context(diagnostic_context const &) = delete;
        diagnostic_context(diagnostic_context&&) = delete;
        diagnostic_context& operator=(diagnostic_context const &) = delete;
        diagnostic_context& operator=(diagnostic_context&&) = delete;
    };

}

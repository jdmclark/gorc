#pragma once

#include "utility/maybe.hpp"

namespace gorc {

    class diagnostic_context_location {
    public:
        maybe<char const *> filename;
        int first_line;
        int first_col;
        int last_line;
        int last_col;

        diagnostic_context_location();
        diagnostic_context_location(maybe<char const *> filename,
                                    int first_line,
                                    int first_col,
                                    int last_line = 0,
                                    int last_col = 0);
    };

}

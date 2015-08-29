#pragma once

namespace gorc {

    enum class log_level {
        error = 1,
        warning = 2,
        info = 4,
        debug = 8,
        trace = 16
    };

    char const* log_level_to_string(log_level);

}

#pragma once

#include "time_span.hpp"

namespace gorc {
inline namespace utility {

class time {
public:
    const gorc::utility::timestamp current;
    const gorc::utility::time_span elapsed_span;

    time(gorc::utility::timestamp current, gorc::utility::timestamp prev);

    inline gorc::utility::timestamp timestamp() const {
        return current;
    }

    inline double elapsed_as_seconds() const {
        return elapsed_span.as_seconds();
    }
};

}
}

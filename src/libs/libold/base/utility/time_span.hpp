#pragma once

#include "timestamp.hpp"

namespace gorc {
inline namespace utility {

class time_span {
private:
    const timestamp begin;
    const timestamp end;
    const double elapsed;

public:
    time_span(timestamp begin, timestamp end);

    inline double as_seconds() const {
        return elapsed;
    }
};

inline time_span operator-(timestamp end, timestamp begin) {
    return time_span(begin, end);
}

}
}

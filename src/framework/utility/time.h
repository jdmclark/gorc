#pragma once

#include <stdint.h>
#include <tuple>

namespace gorc {
inline namespace utility {

class timestamp {
    friend class time;

private:
    uint32_t ts_ms;

public:
    explicit timestamp(uint32_t ts_ms);

    inline timestamp operator+(double dt) const {
        return timestamp(ts_ms + static_cast<uint32_t>(dt * 1000.0));
    }

    inline double operator-(const timestamp& ts) const {
        return (static_cast<double>(ts_ms) - static_cast<double>(ts.ts_ms)) / 1000.0;
    }
};

class time {
private:
    uint32_t current_ts_ms;
    double elapsed_time;

public:
    time(uint32_t current_ts_ms, double elapsed_time);

    inline timestamp get_time_stamp() const {
        return timestamp(current_ts_ms);
    }

    inline operator double() const {
        return elapsed_time;
    }

    inline float in_range(const timestamp& begin, const timestamp& end) {
        return static_cast<float>(static_cast<double>(current_ts_ms - begin.ts_ms) / static_cast<double>(end.ts_ms - begin.ts_ms));
    }

    inline float in_range(const std::tuple<timestamp, timestamp>& range) {
        return in_range(std::get<0>(range), std::get<1>(range));
    }
};

}
}

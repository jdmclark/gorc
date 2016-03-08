#pragma once

#include <stdint.h>

namespace gorc {
inline namespace utility {

class timestamp {
private:
    uint32_t ts_ms;

public:
    explicit timestamp(uint32_t ts_ms);

    inline uint32_t as_milliseconds() const {
        return ts_ms;
    }

    inline bool operator==(const timestamp& ts) const {
        return ts_ms == ts.ts_ms;
    }
};

}
}

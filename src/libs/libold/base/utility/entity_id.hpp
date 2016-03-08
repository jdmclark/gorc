#pragma once

#include <cstdint>

namespace gorc {
inline namespace utility {

class entity_id {
private:
    int32_t value;

public:
    explicit constexpr entity_id(int32_t value)
        : value(value) { }

    inline operator int32_t() const {
        return value;
    }

    inline bool operator==(entity_id v) const {
        return value == v.value;
    }

    inline bool operator!=(entity_id v) const {
        return value != v.value;
    }
};

constexpr entity_id invalid_id(-1);

}
}

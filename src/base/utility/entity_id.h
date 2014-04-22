#pragma once

#include <cstdint>
#include "base/io/binary_input_stream.h"
#include "base/io/binary_output_stream.h"

namespace gorc {
inline namespace utility {

class entity_id {
private:
    int32_t value;

public:
    explicit entity_id(int32_t value);
    entity_id(io::deserialization_constructor_tag, io::binary_input_stream&);

    void serialize(io::binary_output_stream&) const;

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

}
}

#pragma once

#include "base/io/binary_input_stream.h"
#include "base/io/binary_output_stream.h"
#include <algorithm>

namespace gorc {
inline namespace utility {

class fourcc {
private:
    char code[4];

public:
    fourcc(const char* code);
    fourcc(io::deserialization_constructor_tag, io::binary_input_stream&);

    void serialize(io::binary_output_stream&) const;
    void deserialize(io::binary_input_stream&);

    inline bool operator==(const fourcc& f) const {
        return std::equal(code, code + 4, f.code);
    }

    inline bool operator!=(const fourcc& f) const {
        return !std::equal(code, code + 4, f.code);
    }
};

}
}

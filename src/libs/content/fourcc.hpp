#pragma once

#include <cstdint>
#include <algorithm>
#include <string>
#include <stdexcept>
#include "io/binary_input_stream.hpp"
#include "io/binary_output_stream.hpp"

namespace gorc {

    class fourcc {
    private:
        uint32_t data;

    public:
        explicit constexpr fourcc(char const *f)
            : data(0)
        {
            size_t i = 0;
            for(; f[i]; ++i) {
                data <<= 8;
                data |= static_cast<uint32_t>(f[i]);
            }

            if(i > 4) {
                throw std::runtime_error("fourcc size is too large");
            }

            for(; i < 4; ++i) {
                data <<= 8;
            }
        }

        fourcc(deserialization_constructor_tag, binary_input_stream &);

        void binary_serialize_object(binary_output_stream &) const;

        bool operator==(fourcc const &f) const;
        bool operator!=(fourcc const &f) const;

        explicit operator uint32_t() const;
    };

    constexpr fourcc operator"" _4CC(char const *str, size_t)
    {
        return fourcc(str);
    }

    std::string to_string(fourcc);

}

namespace std {

    template <>
    struct hash<gorc::fourcc> {
        size_t operator()(gorc::fourcc const &f) const
        {
            return std::hash<uint32_t>()(static_cast<uint32_t>(f));
        }
    };

}

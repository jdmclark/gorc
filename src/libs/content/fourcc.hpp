#pragma once

#include <cstdint>
#include <algorithm>
#include <string>

namespace gorc {

    class fourcc {
    private:
        uint32_t data;

    public:
        fourcc(char const *f);

        bool operator==(fourcc const &f) const;
        bool operator!=(fourcc const &f) const;

        explicit operator uint32_t() const;
    };

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

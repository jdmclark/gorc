#pragma once

#include <algorithm>

namespace gorc {
inline namespace utility {

class fourcc {
private:
    char code[4];

public:
    fourcc(const char* code);

    inline bool operator==(const fourcc& f) const {
        return std::equal(code, code + 4, f.code);
    }

    inline bool operator!=(const fourcc& f) const {
        return !std::equal(code, code + 4, f.code);
    }
};

}
}

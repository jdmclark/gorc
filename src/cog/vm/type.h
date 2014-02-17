#pragma once

#include <cstdint>

namespace gorc {
namespace cog {
namespace vm {

enum class type : uint8_t {
    nothing,
    integer,
    floating,
    boolean,
    string,
    vector,
    dynamic
};

}
}
}

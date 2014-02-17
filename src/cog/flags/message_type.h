#pragma once

#include <cstdint>

namespace gorc {
namespace flags {

enum class message_type {
    nothing = 0,
    system = 1,
    thing = 3,
    sector = 5,
    surface = 6,
    cog = 9
};

}
}

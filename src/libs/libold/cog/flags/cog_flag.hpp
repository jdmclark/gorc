#pragma once

#include <cstdint>

namespace gorc {
namespace flags {

enum class cog_flag : unsigned int {
    local = 0x40,
    server = 0x80,
    global = 0x100,
    no_sync = 0x200
};

}
}

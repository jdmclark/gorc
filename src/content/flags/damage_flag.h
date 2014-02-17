#pragma once

namespace gorc {
namespace flags {

enum class damage_flag {
    impact                            = 0x1,
    energy                            = 0x2,
    explosion                        = 0x4,
    force                            = 0x8,
    saber                            = 0x10,
    drowning                        = 0x20
};

}
}

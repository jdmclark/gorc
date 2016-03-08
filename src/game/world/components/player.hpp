#pragma once

#include <cstdint>

namespace gorc {
namespace game {
namespace world {
namespace components {

class player {
public:
    #include "player.uid"

    player() = default;
};

}
}
}
}

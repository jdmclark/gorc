#pragma once

#include <cstdint>

namespace gorc {
namespace game {
namespace world {
namespace components {

class character {
public:
    #include "character.uid"

    character() = default;
};

}
}
}
}

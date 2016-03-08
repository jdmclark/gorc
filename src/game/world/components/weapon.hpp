#pragma once

#include <cstdint>

namespace gorc {
namespace game {
namespace world {
namespace components {

class weapon {
public:
    #include "weapon.uid"

    weapon() = default;
};

}
}
}
}

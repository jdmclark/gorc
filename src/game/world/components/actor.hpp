#pragma once

#include <cstdint>

namespace gorc {
namespace game {
namespace world {
namespace components {

class actor {
public:
    #include "actor.uid"

    actor() = default;
};

}
}
}
}

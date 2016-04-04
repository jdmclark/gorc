#pragma once

#include "libold/base/utility/entity_id.hpp"

namespace gorc {
namespace game {
namespace world {
namespace events {

class taken {
public:
    #include "taken.uid"

    int taken_thing;
    int taker;

    taken(int taken_thing, int taker);
};

}
}
}
}

#pragma once

#include "libold/base/utility/entity_id.hpp"

namespace gorc {
namespace game {
namespace world {
namespace events {

class taken {
public:
    #include "taken.uid"

    thing_id taken_thing;
    thing_id taker;

    taken(thing_id taken_thing, thing_id taker);
};

}
}
}
}

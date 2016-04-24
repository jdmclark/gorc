#pragma once

#include "content/id.hpp"

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

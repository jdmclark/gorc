#pragma once

#include "content/id.hpp"

namespace gorc {
namespace game {
namespace world {
namespace events {

class touched_thing {
public:
    #include "touched_thing.uid"

    thing_id toucher;
    thing_id touched;

    touched_thing(thing_id toucher, thing_id touched);
};

}
}
}
}

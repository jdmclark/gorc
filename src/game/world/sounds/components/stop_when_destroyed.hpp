#pragma once

#include "content/id.hpp"

namespace gorc {
namespace game {
namespace world {
namespace sounds {
namespace components {

class stop_when_destroyed {
public:
    #include "stop_when_destroyed.uid"

    thing_id sound;

    stop_when_destroyed(thing_id sound);
};

}
}
}
}
}

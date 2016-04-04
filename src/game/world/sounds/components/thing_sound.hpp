#pragma once

#include "libold/base/utility/entity_id.hpp"

namespace gorc {
namespace game {
namespace world {
namespace sounds {
namespace components {

class thing_sound {
public:
    #include "thing_sound.uid"

    thing_id sound;

    thing_sound(thing_id sound);
};

}
}
}
}
}

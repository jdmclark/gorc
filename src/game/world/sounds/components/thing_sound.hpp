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

    entity_id sound;

    thing_sound(entity_id sound);
};

}
}
}
}
}

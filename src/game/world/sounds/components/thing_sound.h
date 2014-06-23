#pragma once

#include "base/utility/entity_id.h"

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

#pragma once

#include "libold/base/utility/entity_id.hpp"
#include "libold/content/flags/sound_subclass_type.hpp"

namespace gorc {
namespace game {
namespace world {
namespace events {

class class_sound {
public:
    #include "class_sound.uid"

    thing_id thing;
    flags::sound_subclass_type type;

    class_sound(thing_id thing, flags::sound_subclass_type type);
};

}
}
}
}

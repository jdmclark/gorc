#pragma once

#include "base/utility/entity_id.h"
#include "content/flags/sound_subclass_type.h"

namespace gorc {
namespace game {
namespace world {
namespace events {

class class_sound {
public:
    #include "class_sound.uid"

    entity_id thing;
    flags::sound_subclass_type type;

    class_sound(entity_id thing, flags::sound_subclass_type type);
};

}
}
}
}

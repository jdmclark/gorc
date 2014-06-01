#include "class_sounds.h"

using gorc::game::world::components::class_sounds;

class_sounds::class_sounds(content::assets::soundclass const &soundclass,
                           flags::standing_material_type standing_material_type)
    : soundclass(soundclass)
    , standing_material_type(standing_material_type) {
    return;
}

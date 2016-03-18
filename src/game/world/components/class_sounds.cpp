#include "class_sounds.hpp"

using gorc::game::world::components::class_sounds;

class_sounds::class_sounds(asset_ref<content::assets::soundclass> soundclass,
                           flags::standing_material_type standing_material_type)
    : soundclass(soundclass)
    , standing_material_type(standing_material_type) {
    return;
}

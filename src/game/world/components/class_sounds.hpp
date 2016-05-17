#pragma once

#include "libold/content/assets/soundclass.hpp"
#include "game/flags/standing_material_type.hpp"
#include "utility/uid.hpp"

namespace gorc {
namespace game {
namespace world {
namespace components {

class class_sounds {
public:
    uid(878230975);

    asset_ref<content::assets::soundclass> soundclass;
    flags::standing_material_type standing_material_type;

    class_sounds(asset_ref<content::assets::soundclass> soundclass,
                 flags::standing_material_type = flags::standing_material_type::none);
};

}
}
}
}

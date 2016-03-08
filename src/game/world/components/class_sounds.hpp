#pragma once

#include "libold/content/assets/soundclass.hpp"
#include "game/flags/standing_material_type.hpp"

namespace gorc {
namespace game {
namespace world {
namespace components {

class class_sounds {
public:
    #include "class_sounds.uid"

    content::assets::soundclass const &soundclass;
    flags::standing_material_type standing_material_type;

    class_sounds(content::assets::soundclass const &soundclass,
                 flags::standing_material_type = flags::standing_material_type::none);
};

}
}
}
}

#pragma once

#include "libold/content/assets/puppet.hpp"
#include "game/flags/standing_material_type.hpp"

namespace gorc {
namespace game {
namespace world {
namespace components {

class puppet_animations {
public:
    #include "puppet_animations.uid"

    asset_ref<content::assets::puppet> puppet;
    flags::puppet_mode_type puppet_mode_type;
    int actor_walk_animation = -1;

    puppet_animations(asset_ref<content::assets::puppet> puppet,
                      flags::puppet_mode_type = flags::puppet_mode_type::unarmed);
};

}
}
}
}

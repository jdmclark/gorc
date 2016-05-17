#pragma once

#include "libold/content/assets/puppet.hpp"
#include "game/flags/standing_material_type.hpp"
#include "utility/uid.hpp"

namespace gorc {
namespace game {
namespace world {
namespace components {

class puppet_animations {
public:
    uid(204472411);

    asset_ref<content::assets::puppet> puppet;
    flags::puppet_mode_type puppet_mode_type;
    maybe<thing_id> actor_walk_animation;

    puppet_animations(asset_ref<content::assets::puppet> puppet,
                      flags::puppet_mode_type = flags::puppet_mode_type::unarmed);
};

}
}
}
}

#include "puppet_animations.hpp"

using gorc::game::world::components::puppet_animations;

puppet_animations::puppet_animations(asset_ref<content::assets::puppet> puppet,
                                     flags::puppet_mode_type puppet_mode_type)
    : puppet(puppet)
    , puppet_mode_type(puppet_mode_type) {
    return;
}

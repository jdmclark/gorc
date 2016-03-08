#include "puppet_animations.hpp"

using gorc::game::world::components::puppet_animations;

puppet_animations::puppet_animations(content::assets::puppet const &puppet,
                                     flags::puppet_mode_type puppet_mode_type)
    : puppet(puppet)
    , puppet_mode_type(puppet_mode_type) {
    return;
}

#include "surface_light.hpp"

gorc::game::world::animations::components::surface_light::surface_light(int surface, float start_light, float end_light, float change_time)
    : surface(surface), start_light(start_light), end_light(end_light), change_time(change_time), anim_time(0.0f) {
    return;
}

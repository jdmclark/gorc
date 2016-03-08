#include "surface_material.hpp"

gorc::game::world::animations::components::surface_material::surface_material(int surface, double framerate, flag_set<flags::anim_flag> flag)
    : surface(surface), framerate(framerate), flag(flag) {
    return;
}

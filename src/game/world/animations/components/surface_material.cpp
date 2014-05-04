#include "surface_material.h"

gorc::game::world::animations::components::surface_material::surface_material(int surface, double framerate, flag_set<flags::anim_flag> flag)
    : surface(surface), framerate(framerate), flag(flag) {
    return;
}

gorc::game::world::animations::components::surface_material::surface_material(io::deserialization_constructor_tag, io::binary_input_stream& is) {
    surface = io::deserialize<int>(is);
    framerate = io::deserialize<double>(is);
    flag = io::deserialize<flag_set<flags::anim_flag>>(is);
    framerate_accumulator = io::deserialize<double>(is);
}

void gorc::game::world::animations::components::surface_material::serialize(io::binary_output_stream& os) const {
    io::serialize(os, surface);
    io::serialize(os, framerate);
    io::serialize(os, flag);
    io::serialize(os, framerate_accumulator);
}

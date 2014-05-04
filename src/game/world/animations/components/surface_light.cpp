#include "surface_light.h"

gorc::game::world::animations::components::surface_light::surface_light(int surface, float start_light, float end_light, float change_time)
    : surface(surface), start_light(start_light), end_light(end_light), change_time(change_time), anim_time(0.0f) {
    return;
}

gorc::game::world::animations::components::surface_light::surface_light(io::deserialization_constructor_tag, io::binary_input_stream& is) {
    surface = io::deserialize<int>(is);
    start_light = io::deserialize<float>(is);
    end_light = io::deserialize<float>(is);
    change_time = io::deserialize<float>(is);
    anim_time = io::deserialize<float>(is);
}

void gorc::game::world::animations::components::surface_light::serialize(io::binary_output_stream& os) const {
    io::serialize(os, surface);
    io::serialize(os, start_light);
    io::serialize(os, end_light);
    io::serialize(os, change_time);
    io::serialize(os, anim_time);
}

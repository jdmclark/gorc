#include "surface_animation.h"

gorc::game::world::animations::components::surface_animation::surface_animation(int surface)
    : surface(surface) {
    return;
}

gorc::game::world::animations::components::surface_animation::surface_animation(io::deserialization_constructor_tag, io::binary_input_stream& is)
    : surface(io::deserialize<int>(is)) {
    return;
}

void gorc::game::world::animations::components::surface_animation::serialize(io::binary_output_stream& os) const {
    io::serialize(os, surface);
}

#include "surface_animation.h"

gorc::game::world::animations::components::surface_animation::surface_animation(entity_id animation)
    : animation(animation) {
    return;
}

gorc::game::world::animations::components::surface_animation::surface_animation(io::deserialization_constructor_tag, io::binary_input_stream& is)
    : animation(io::deserialize<entity_id>(is)) {
    return;
}

void gorc::game::world::animations::components::surface_animation::serialize(io::binary_output_stream& os) const {
    io::serialize(os, animation);
}

#include "slide_ceiling_sky.h"

gorc::game::world::animations::components::slide_ceiling_sky::slide_ceiling_sky(const vector<2>& speed) : speed(speed) {
    return;
}

gorc::game::world::animations::components::slide_ceiling_sky::slide_ceiling_sky(io::deserialization_constructor_tag tag, io::binary_input_stream& is)
    : speed(tag, is) {
    return;
}

void gorc::game::world::animations::components::slide_ceiling_sky::serialize(io::binary_output_stream& os) const {
    io::serialize(os, speed);
}

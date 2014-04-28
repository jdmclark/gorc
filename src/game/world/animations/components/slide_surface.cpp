#include "slide_surface.h"

gorc::game::world::animations::components::slide_surface::slide_surface(entity_id surface, const vector<3>& direction,
        const vector<3>& sb0, const vector<3>& sb1, const vector<2>& tb0, const vector<2>& tb1)
    : surface(surface), direction(direction), sb0(sb0), sb1(sb1), tb0(tb0), tb1(tb1) {
    return;
}

gorc::game::world::animations::components::slide_surface::slide_surface(io::deserialization_constructor_tag tag, io::binary_input_stream& is)
    : surface(tag, is), direction(tag, is), sb0(tag, is), sb1(tag, is), tb0(tag, is), tb1(tag, is) {
    return;
}

void gorc::game::world::animations::components::slide_surface::serialize(io::binary_output_stream& os) const {
    io::serialize(os, surface);
    io::serialize(os, direction);
    io::serialize(os, sb0);
    io::serialize(os, sb1);
    io::serialize(os, tb0);
    io::serialize(os, tb1);
}

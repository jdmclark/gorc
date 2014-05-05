#include "character.h"

gorc::game::world::components::character::character(io::deserialization_constructor_tag, io::binary_input_stream&) {
    return;
}

void gorc::game::world::components::character::serialize(io::binary_output_stream&) const {
    return;
}

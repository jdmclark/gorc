#include "character.h"

using gorc::game::world::components::character;

character::character(io::deserialization_constructor_tag,
                     io::binary_input_stream &) {
    return;
}

void character::serialize(io::binary_output_stream &) const {
    return;
}

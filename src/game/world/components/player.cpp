#include "player.h"

gorc::game::world::components::player::player(io::deserialization_constructor_tag, io::binary_input_stream&) {
    return;
}

void gorc::game::world::components::player::serialize(io::binary_output_stream&) const {
    return;
}

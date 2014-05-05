#include "actor.h"

gorc::game::world::components::actor::actor(io::deserialization_constructor_tag, io::binary_input_stream&) {
    return;
}

void gorc::game::world::components::actor::serialize(io::binary_output_stream&) const {
    return;
}

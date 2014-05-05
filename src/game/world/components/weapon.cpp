#include "weapon.h"

gorc::game::world::components::weapon::weapon(io::deserialization_constructor_tag, io::binary_input_stream&) {
    return;
}

void gorc::game::world::components::weapon::serialize(io::binary_output_stream&) const {
    return;
}

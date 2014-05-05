#include "item.h"

gorc::game::world::components::item::item(io::deserialization_constructor_tag, io::binary_input_stream&) {
    return;
}

void gorc::game::world::components::item::serialize(io::binary_output_stream&) const {
    return;
}

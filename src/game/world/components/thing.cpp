#include "thing.h"

gorc::game::world::components::thing::thing(const content::assets::thing_template& tpl)
    : content::assets::thing_template(tpl) {
    return;
}

gorc::game::world::components::thing::thing(io::deserialization_constructor_tag, io::binary_input_stream&) {
    // TODO
    return;
}

void gorc::game::world::components::thing::serialize(io::binary_output_stream&) const {
    // TODO
    return;
}

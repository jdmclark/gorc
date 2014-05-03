#include "thing.h"

gorc::game::world::thing::thing(const content::assets::thing_template& tpl)
    : content::assets::thing_template(tpl) {
    return;
}

gorc::game::world::thing::thing(io::deserialization_constructor_tag, io::binary_input_stream&) {
    // TODO
    return;
}

void gorc::game::world::thing::serialize(io::binary_output_stream&) const {
    // TODO
    return;
}

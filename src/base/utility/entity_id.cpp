#include "entity_id.h"

gorc::entity_id::entity_id(io::deserialization_constructor_tag, io::binary_input_stream& is) {
    value = io::deserialize<int32_t>(is);
}

void gorc::entity_id::serialize(io::binary_output_stream& os) const {
    io::serialize(os, value);
}

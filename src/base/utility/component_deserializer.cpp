#include "component_deserializer.h"

gorc::utility::component_type_deserializer::component_type_deserializer(component_deserialization_fn d_fn)
    : d_fn(d_fn) {
    return;
}

void gorc::utility::component_type_deserializer::deserialize(component_system &cs,
                                                             entity_id id,
                                                             io::binary_input_stream &is) {
    d_fn(cs, id, is);
}

gorc::utility::base_component_type_deserializer& gorc::utility::component_deserializer::get_deserializer(uint32_t uid) {
    auto it = deserializers.find(uid);
    if(it != deserializers.end()) {
        return *it->second;
    }

    throw std::exception();
}

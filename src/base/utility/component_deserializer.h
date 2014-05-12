#pragma once

#include "base/utility/component_system.h"
#include "base/io/binary_input_stream.h"
#include "base/utility/make_unique.h"
#include <functional>
#include <unordered_map>
#include <memory>

namespace gorc {
inline namespace utility {

using component_deserialization_fn = std::function<void(component_system&, entity_id, io::binary_input_stream&)>;

class component_type_deserializer : public base_component_type_deserializer {
private:
    component_deserialization_fn d_fn;
public:
    component_type_deserializer(component_deserialization_fn d_fn);

    virtual void deserialize(component_system &, entity_id, io::binary_input_stream &) override;
};

class component_deserializer : public base_component_deserializer {
private:
    std::unordered_map<uint32_t, std::unique_ptr<component_type_deserializer>> deserializers;

public:
    template <typename CompT>
    void add_deserializer(component_deserialization_fn d_fn) {
        deserializers.emplace(static_cast<uint32_t>(CompT::UID::UID),
                              make_unique<component_type_deserializer>(d_fn));
    }

    template <typename CompT>
    void add_default_deserializer() {
        add_deserializer<CompT>([](component_system &cs,
                                   entity_id id,
                                   io::binary_input_stream &is) {
            cs.emplace_component<CompT>(id, io::deserialization_constructor, is);
        });
    }

    virtual base_component_type_deserializer& get_deserializer(uint32_t component_uid) override;
};

}
}

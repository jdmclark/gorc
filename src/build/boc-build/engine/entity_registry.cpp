#include "entity_registry.hpp"

gorc::entity_registry::entity_registry_element::~entity_registry_element()
{
    return;
}

gorc::entity* gorc::entity_registry::deserialize(uint32_t entity_type,
                                                 entity_input_stream &ins,
                                                 entity_allocator &alloc) const
{
    return entity_elements_map.at(entity_type)->deserialize(ins, alloc);
}

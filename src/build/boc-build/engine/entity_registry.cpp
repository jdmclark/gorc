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

uint32_t gorc::entity_registry::get_type_id(std::type_index tid) const
{
    return entity_type_map.at(tid);
}

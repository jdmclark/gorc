#include "entity_serializer.hpp"
#include "entity.hpp"
#include "log/log.hpp"

gorc::entity_output_stream::entity_output_stream(entity_registry const &reg,
                                                 output_stream &os)
    : reg(reg)
    , os(os)
{
    return;
}

gorc::entity_output_stream::~entity_output_stream()
{
    return;
}

void gorc::entity_output_stream::write_entity_reference(entity *ent)
{
    auto it = entity_id_map.find(ent);
    if(it == entity_id_map.end()) {
        LOG_FATAL(format("cannot write database: reference to unserialized entity '%s'") %
                  ent->name());
    }

    write_uint32(it->second);
}

void gorc::entity_output_stream::write_string(std::string const &str)
{
    write_uint32(static_cast<uint32_t>(str.size()));
    os.write(str.data(), str.size());
}

void gorc::entity_output_stream::write_path(path const &p)
{
    write_string(p.generic_string());
}

void gorc::entity_output_stream::write_uint32(uint32_t value)
{
    write<uint32_t>(os, value);
}

void gorc::entity_output_stream::write_bool(bool b)
{
    write<uint32_t>(os, b ? 1 : 0);
}

gorc::entity_serializer::entity_serializer(entity_registry const &reg,
                                           output_stream &os)
    : entity_output_stream(reg, os)
{
    return;
}

void gorc::entity_serializer::serialize_entity(entity *ent)
{
    if(closed_entities.find(ent) != closed_entities.end()) {
        LOG_FATAL(format("cannot write database: cycle detected in entity '%s'") %
                  ent->name());
    }

    if(entity_id_map.find(ent) != entity_id_map.end()) {
        // Skip serializing entities that are already serialized
        return;
    }

    closed_entities.insert(ent);

    // First serialize all dependencies
    for(entity *dep : ent->dependencies()) {
        serialize_entity(dep);
    }

    // Write entity type id before the entity body
    write_uint32(static_cast<uint32_t>(reg.get_type_id(ent->get_type_index())));

    ent->serialize(*this);
    entity_id_map.emplace(ent, static_cast<uint32_t>(entity_id_map.size()));
    closed_entities.erase(ent);
}

void gorc::entity_serializer::serialize_root(entity *ent)
{
    serialize_entity(ent);
}

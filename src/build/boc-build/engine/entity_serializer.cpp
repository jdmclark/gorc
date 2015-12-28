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

void gorc::entity_output_stream::write_entity_reference(entity* ent)
{
    auto it = entity_id_map.find(ent);
    if(it == entity_id_map.end()) {
        LOG_FATAL(format("cycle found in entity '%s'") % ent->name());
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

gorc::entity_serializer::entity_serializer(entity_registry const &reg,
                                           output_stream &os)
    : entity_output_stream(reg, os)
{
    return;
}

void gorc::entity_serializer::serialize_entity(entity *ent)
{
    // First serialize all dependencies
    for(entity *dep : ent->dependencies()) {
        serialize_entity(dep);
    }

    ent->serialize(*this);
}

void gorc::entity_serializer::serialize_root(entity *ent)
{
    serialize_entity(ent);
}

#include "entity_deserializer.hpp"
#include "entity.hpp"
#include "utility/maybe.hpp"
#include "log/log.hpp"

gorc::entity_input_stream::entity_input_stream(service_registry const &services, input_stream &is)
    : is(is)
    , services(services)
{
    return;
}

gorc::entity_input_stream::~entity_input_stream()
{
    return;
}

gorc::entity* gorc::entity_input_stream::read_abstract_entity_reference()
{
    return entity_id_map.at(read_uint32());
}

std::string gorc::entity_input_stream::read_string()
{
    std::string str;

    uint32_t length = read_uint32();
    str.resize(length);

    is.read(&str[0], length);

    return str;
}

gorc::path gorc::entity_input_stream::read_path()
{
    return path(read_string());
}

uint32_t gorc::entity_input_stream::read_uint32()
{
    return read<uint32_t>(is);
}

gorc::entity_deserializer::entity_deserializer(service_registry const &services,
                                               entity_registry const &reg,
                                               entity_allocator &alloc,
                                               input_stream &is)
    : entity_input_stream(services, is)
    , reg(reg)
    , alloc(alloc)
{
    return;
}

gorc::entity* gorc::entity_deserializer::deserialize_abstract_root()
{
    maybe<gorc::entity*> last_root;
    while(true) {
        uint32_t entity_type_id;
        auto read_sz = is.read_some(&entity_type_id, sizeof(entity_type_id));
        if(read_sz == 0 && is.at_end()) {
            break;
        }
        else if(read_sz != sizeof(entity_type_id)) {
            LOG_FATAL("database corrupt: could not read next entity type id");
        }

        entity *new_entity = reg.deserialize(entity_type_id, *this, alloc);

        entity_id_map.emplace(entity_id_map.size(), new_entity);

        last_root = new_entity;
    }

    if(!last_root.has_value()) {
        LOG_FATAL("database corrupt: no root node exists");
    }

    return last_root.get_value();
}

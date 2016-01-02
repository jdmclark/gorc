#pragma once

#include "entity_registry.hpp"
#include "io/output_stream.hpp"
#include "io/path.hpp"
#include <unordered_map>
#include <unordered_set>

namespace gorc {

    class entity;

    class entity_output_stream {
    protected:
        entity_registry const &reg;
        output_stream &os;
        std::unordered_set<entity*> closed_entities;
        std::unordered_map<entity*, uint32_t> entity_id_map;

    public:
        entity_output_stream(entity_registry const &reg, output_stream &os);
        virtual ~entity_output_stream();

        void write_entity_reference(entity*);
        void write_string(std::string const &);
        void write_path(path const &);
        void write_uint32(uint32_t value);

        template <typename EntitySetT>
        void write_entity_set(EntitySetT const &set)
        {
            write_uint32(static_cast<uint32_t>(set.size()));
            for(entity *ent : set) {
                write_entity_reference(ent);
            }
        }
    };

    class entity_serializer : private entity_output_stream {
    private:
        void serialize_entity(entity *ent);

    public:
        entity_serializer(entity_registry const &reg, output_stream &os);

        void serialize_root(entity *ent);
    };

}

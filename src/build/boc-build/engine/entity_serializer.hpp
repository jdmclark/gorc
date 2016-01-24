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
        void write_bool(bool b);

        template <typename RangeT, typename TransformFnT>
        void write_range(RangeT const &rng, TransformFnT trns_fn)
        {
            write_uint32(static_cast<uint32_t>(rng.size()));
            for(auto const &em : rng) {
                trns_fn(em, *this);
            }
        }

        template <typename EntitySetT>
        void write_entity_set(EntitySetT const &set)
        {
            write_range(set, [](entity *em, entity_output_stream &os) {
                    os.write_entity_reference(em);
                });
        }

        template <typename EnumRangeT>
        void write_enum_set(EnumRangeT const &els)
        {
            write_range(els, [](auto t, auto &os) {
                    os.write_uint32(static_cast<uint32_t>(t));
                });
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

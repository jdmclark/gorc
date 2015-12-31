#pragma once

#include "entity_registry.hpp"
#include "entity_allocator.hpp"
#include "io/input_stream.hpp"
#include "io/path.hpp"
#include "log/log.hpp"
#include "utility/service_registry.hpp"
#include <unordered_map>

namespace gorc {

    class entity;

    class entity_input_stream {
    protected:
        input_stream &is;
        std::unordered_map<uint32_t, entity*> entity_id_map;

    public:
        service_registry const &services;

        entity_input_stream(service_registry const &services, input_stream &is);
        virtual ~entity_input_stream();

        entity* read_abstract_entity_reference();

        template <typename T>
        T* read_entity_reference()
        {
            entity *abstract_ptr = read_abstract_entity_reference();
            T *typed_ptr = dynamic_cast<T*>(abstract_ptr);

            if(typed_ptr == nullptr) {
                LOG_FATAL(format("database corrupt: entity '%s' type mismatch") %
                          abstract_ptr->name());
            }

            return typed_ptr;
        }

        std::string read_string();
        path read_path();
        uint32_t read_uint32();

        template <typename EntityT>
        std::unordered_set<EntityT*> read_entity_set()
        {
            std::unordered_set<EntityT*> rv;

            uint32_t count = read_uint32();
            for(uint32_t i = 0; i < count; ++i) {
                rv.insert(read_entity_reference<EntityT>());
            }

            return rv;
        }
    };

    class entity_deserializer : private entity_input_stream {
    private:
        entity_registry const &reg;
        entity_allocator &alloc;

        entity* deserialize_abstract_root();

    public:
        entity_deserializer(service_registry const &services,
                            entity_registry const &reg,
                            entity_allocator &,
                            input_stream &is);

        template <typename T>
        T* deserialize_root()
        {
            entity *abstract_root = deserialize_abstract_root();
            T *typed_root = dynamic_cast<T*>(abstract_root);

            if(typed_root == nullptr) {
                LOG_FATAL("database corrupt: root node type mismatch");
            }

            return typed_root;
        }
    };

}

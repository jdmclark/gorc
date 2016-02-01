#pragma once

#include "entity_registry.hpp"
#include "entity_allocator.hpp"
#include "io/binary_input_stream.hpp"
#include "io/path.hpp"
#include "log/log.hpp"
#include "utility/service_registry.hpp"
#include <unordered_map>
#include <type_traits>

namespace gorc {

    class entity;

    class entity_input_stream {
    protected:
        binary_input_stream is;
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
        bool read_bool();

        template <typename ResultT, typename InsertFnT>
        ResultT read_range(InsertFnT ins_fn)
        {
            ResultT rv;

            uint32_t count = read_uint32();
            for(uint32_t i = 0; i < count; ++i) {
                ins_fn(rv, *this);
            }

            return rv;
        }

        template <typename EntityT>
        std::unordered_set<EntityT*> read_entity_set()
        {
            return read_range<std::unordered_set<EntityT*>>(
                    [](std::unordered_set<EntityT*> &rv, entity_input_stream &is) {
                        rv.insert(is.read_entity_reference<EntityT>());
                    });
        }

        template <typename EnumRangeT>
        EnumRangeT read_enum_set()
        {
            return read_range<EnumRangeT>([](auto &rv, auto &is) {
                    rv.insert(static_cast<typename std::decay<decltype(*rv.begin())>::type>(
                                is.read_uint32()));
                });
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

#pragma once

#include <typeindex>
#include <vector>
#include <memory>
#include <unordered_map>
#include <cstdint>
#include "entity_allocator.hpp"

namespace gorc {

    class entity;
    class entity_input_stream;

    class entity_registry {
    private:
        class entity_registry_element {
        public:
            virtual ~entity_registry_element();

            virtual entity* deserialize(entity_input_stream &,
                                        entity_allocator &) = 0;
        };

        template <typename T>
        class entity_registry_element_impl : public entity_registry_element {
        public:
            virtual entity* deserialize(entity_input_stream &instr,
                                        entity_allocator &alloc) override
            {
                return alloc.emplace<T>(instr);
            }
        };

        std::vector<std::unique_ptr<entity_registry_element>> entity_types;
        std::unordered_map<std::type_index, uint32_t> entity_type_map;
        std::unordered_map<uint32_t, entity_registry_element*> entity_elements_map;

    public:
        template <typename T>
        void register_entity()
        {
            uint32_t next_id = static_cast<uint32_t>(entity_types.size());
            entity_types.push_back(make_unique<entity_registry_element_impl<T>>());
            entity_type_map.emplace(typeid(T), next_id);
            entity_elements_map.emplace(next_id, entity_types.back().get());
        }

        template <typename T>
        uint32_t get_type_id() const
        {
            return entity_type_map.at(typeid(T));
        }

        uint32_t get_type_id(std::type_index) const;

        entity* deserialize(uint32_t type,
                            entity_input_stream &,
                            entity_allocator &) const;
    };

}

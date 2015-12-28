#pragma once

#include <typeindex>
#include <vector>
#include <memory>
#include <unordered_map>
#include <cstdint>
#include "utility/make_unique.hpp"

namespace gorc {

    class entity_registry {
    private:
        class entity_registry_element {
        public:
            virtual ~entity_registry_element();
        };

        template <typename T>
        class entity_registry_element_impl : public entity_registry_element {
        };

        std::vector<std::unique_ptr<entity_registry_element>> entity_types;
        std::unordered_map<std::type_index, uint32_t> entity_type_map;

    public:
        template <typename T>
        void register_entity()
        {
            uint32_t next_id = static_cast<uint32_t>(entity_types.size());
            entity_types.push_back(make_unique<entity_registry_element_impl<T>>());
            entity_type_map.emplace(typeid(T), next_id);
        }

        template <typename T>
        uint32_t get_type_id() const
        {
            return entity_type_map.at(typeid(T));
        }
    };

}

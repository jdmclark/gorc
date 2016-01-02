#pragma once

#include <string>
#include <unordered_set>
#include <typeindex>
#include <type_traits>
#include "utility/service_registry.hpp"

namespace gorc {

    class entity_input_stream;
    class entity_output_stream;

    class entity {
    public:
        virtual ~entity();

        virtual std::string const& name() = 0;
        virtual std::unordered_set<entity*> const& dependencies() = 0;

        virtual bool is_dirty() = 0;
        virtual bool update(service_registry const &services) = 0;

        virtual void serialize(entity_output_stream &) = 0;
        virtual std::type_index get_type_index() const = 0;
    };

}

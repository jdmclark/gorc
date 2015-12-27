#pragma once

#include <string>
#include <unordered_set>
#include "utility/service_registry.hpp"

namespace gorc {

    class entity {
    public:
        virtual ~entity();

        virtual std::string const& name() = 0;
        virtual std::unordered_set<entity*> const& dependencies() = 0;

        virtual bool is_dirty() = 0;
        virtual bool update(service_registry const &services) = 0;
    };

}

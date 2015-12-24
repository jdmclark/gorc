#pragma once

#include <string>
#include <unordered_set>

namespace gorc {

    class entity {
    public:
        virtual ~entity();

        virtual std::string const& name() = 0;
        virtual std::unordered_set<entity*> dependencies() = 0;

        virtual bool is_dirty() = 0;
        virtual bool update() = 0;
    };

}

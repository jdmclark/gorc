#pragma once

#include "content/id.hpp"

namespace gorc {

    template <typename IdT>
    class entity_destroyed {
    public:
        IdT entity;

        entity_destroyed(IdT entity)
            : entity(entity)
        {
            return;
        }
    };

}

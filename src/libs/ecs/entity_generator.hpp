#pragma once

#include "content/id.hpp"

namespace gorc {

    template <typename IdT>
    class entity_generator {
    public:
        virtual ~entity_generator()
        {
            return;
        }

        virtual IdT emplace() = 0;
        virtual void erase(IdT entity) = 0;

        virtual void flush_erase_queue() = 0;
    };

}

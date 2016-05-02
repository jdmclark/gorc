#pragma once

#include "content/id.hpp"

namespace gorc {

    template <typename IdT>
    class abstract_component_pool {
    public:
        virtual ~abstract_component_pool()
        {
            return;
        }

        virtual void erase_equal_range(IdT entity) = 0;
        virtual void flush_erase_queue() = 0;
    };

}

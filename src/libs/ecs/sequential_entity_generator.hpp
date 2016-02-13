#pragma once

#include "entity_generator.hpp"

namespace gorc {

    template <typename IdT>
    class sequential_entity_generator : public entity_generator<IdT> {
    private:
        using IdValueT = typename std::underlying_type<IdT>::type;
        IdValueT next = 0;

    public:
        virtual IdT emplace() override
        {
            return IdT(next++);
        }

        virtual void erase(IdT) override
        {
            return;
        }
    };

}

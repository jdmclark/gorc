#pragma once

#include "entity.hpp"
#include "utility/make_unique.hpp"
#include <vector>

namespace gorc {

    class entity_allocator {
    private:
        std::vector<std::unique_ptr<entity>> entities;

    public:
        template <typename T, typename ...ArgT>
        T* emplace(ArgT &&...args)
        {
            auto ptr = make_unique<T>(std::forward<ArgT>(args)...);
            T* rv = ptr.get();
            entities.push_back(std::move(ptr));
            return rv;
        }
    };

}

#pragma once

#include "component_pool.hpp"
#include "utility/maybe.hpp"

#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <utility>

namespace gorc {

    template <typename IdT, size_t page_size = 128>
    class component_relational_mapping {
    private:
        template <typename CompT>
        using CompPoolT = component_pool<IdT, CompT, page_size>;

        std::unordered_map<std::type_index, std::unique_ptr<abstract_component_pool<IdT>>> pools;

        template <typename CompT>
        CompPoolT<CompT>& get_or_create_pool()
        {
            auto it = pools.find(typeid(CompT));
            if(it == pools.end()) {
                it = pools.emplace(typeid(CompT), std::make_unique<CompPoolT<CompT>>()).first;
            }

            return *reinterpret_cast<CompPoolT<CompT>*>(it->second.get());
        }

    public:
        template <typename CompT, typename ...ArgT>
        CompT& emplace(IdT entity, ArgT &&...args)
        {
            return get_or_create_pool<CompT>().emplace(entity, std::forward<ArgT>(args)...);
        }

        template <typename CompT>
        auto range()
        {
            auto &pool = get_or_create_pool<CompT>();
            return make_range(pool.begin(), pool.end());
        }

        template <typename CompT>
        auto equal_range(IdT entity)
        {
            return get_or_create_pool<CompT>().equal_range(entity);
        }

        template <typename IteratorT, typename = decltype(*std::declval<IteratorT>())>
        auto erase(IteratorT it)
        {
            using CompT = typename std::decay<decltype(*it->second)>::type;
            return get_or_create_pool<CompT>().erase(it);
        }

        template <typename IteratorT>
        auto erase(IteratorT first, IteratorT last)
        {
            using CompT = typename std::decay<decltype(*first->second)>::type;
            return get_or_create_pool<CompT>().erase(first, last);
        }

        void erase_equal_range(IdT entity)
        {
            for(auto &pool : pools) {
                pool.second->erase_equal_range(entity);
            }
        }
    };

}

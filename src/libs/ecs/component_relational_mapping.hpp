#pragma once

#include "component_pool.hpp"
#include "utility/maybe.hpp"
#include "utility/uid.hpp"
#include "log/log.hpp"

#include <type_traits>
#include <unordered_map>
#include <utility>

namespace gorc {

    template <typename IdT>
    class component_relational_mapping {
    private:
        template <typename CompT>
        using CompPoolT = component_pool<IdT, CompT>;

        std::unordered_map<uint32_t, std::unique_ptr<abstract_component_pool<IdT>>> pools;

        template <typename CompT>
        CompPoolT<CompT>& get_pool() const
        {
            auto it = pools.find(uid_of<CompT>());
            if(it == pools.end()) {
                LOG_FATAL(format("component type with uid %d is not registered") %
                          uid_of<CompT>());
            }

            return *reinterpret_cast<CompPoolT<CompT>*>(it->second.get());
        }

    public:
        template <typename CompT>
        void register_component_type()
        {
            auto res = pools.emplace(uid_of<CompT>(), std::make_unique<CompPoolT<CompT>>());
            if(!res.second) {
                LOG_FATAL(format("component type with uid %d is already registered") %
                          uid_of<CompT>());
            }
        }

        template <typename CompT, typename ...ArgT>
        CompT& emplace(IdT entity, ArgT &&...args)
        {
            return get_pool<CompT>().emplace(entity, std::forward<ArgT>(args)...);
        }

        template <typename CompT>
        auto range()
        {
            auto &pool = get_pool<CompT>();
            return make_range(pool.begin(), pool.end());
        }

        template <typename CompT>
        auto equal_range(IdT entity)
        {
            return get_pool<CompT>().equal_range(entity);
        }

        template <typename IteratorT, typename = decltype(*std::declval<IteratorT>())>
        auto erase(IteratorT it)
        {
            using CompT = typename std::decay<decltype(*it->second)>::type;
            return get_pool<CompT>().erase(it);
        }

        template <typename IteratorT>
        auto erase(IteratorT first, IteratorT last)
        {
            using CompT = typename std::decay<decltype(*first->second)>::type;
            return get_pool<CompT>().erase(first, last);
        }

        template <typename CompT, typename PredT>
        void erase_if(PredT pred)
        {
            get_pool<CompT>().erase_if(pred);
        }

        void erase_equal_range(IdT entity)
        {
            for(auto &pool : pools) {
                pool.second->erase_equal_range(entity);
            }
        }

        void flush_erase_queue()
        {
            for(auto &pool : pools) {
                pool.second->flush_erase_queue();
            }
        }
    };

}

#pragma once

#include "pool.hpp"
#include "utility/range.hpp"
#include "abstract_component_pool.hpp"
#include <unordered_map>
#include <utility>

namespace gorc {

    template <typename IdT, typename CompT, size_t page_size = 128>
    class component_pool : public abstract_component_pool<IdT> {
    private:
        using PoolT = pool<CompT, page_size>;
        using IndexT = std::unordered_multimap<IdT, CompT*>;

    public:
        using iterator = typename IndexT::iterator;
        using const_iterator = typename IndexT::const_iterator;

    private:
        PoolT components;
        IndexT index;
        std::unordered_map<CompT*, const_iterator> erase_queue;

    public:
        auto begin()
        {
            return index.begin();
        }

        auto end()
        {
            return index.end();
        }

        template <typename ...ArgT>
        CompT& emplace(IdT parent, ArgT &&...args)
        {
            auto &em = components.emplace(std::forward<ArgT>(args)...);
            index.emplace(parent, &em);
            return em;
        }

        auto erase(const_iterator it)
        {
            erase_queue.emplace(it->second, it);
            return ++it;
        }

        auto erase(const_iterator begin, const_iterator end)
        {
            for(auto it = begin; it != end; ++it) {
                erase_queue.emplace(it->second, it);
            }

            return end;
        }

        auto erase(range<const_iterator> const &rng)
        {
            erase(rng.begin(), rng.end());
            return rng.end();
        }

        auto erase(range<iterator> const &rng)
        {
            erase(rng.begin(), rng.end());
            return rng.end();
        }

        range<iterator> equal_range(IdT id)
        {
            return make_range(index.equal_range(id));
        }

        range<const_iterator> equal_range(IdT id) const
        {
            return make_range(index.equal_range(id));
        }

        virtual void erase_equal_range(IdT id) override
        {
            erase(equal_range(id));
        }

        template <typename PredT>
        void erase_if(PredT pred)
        {
            for(const_iterator it = index.begin(); it != index.end(); ) {
                bool should_delete = pred(it->first, *it->second);
                if(should_delete) {
                    it = erase(it);
                }
                else {
                    ++it;
                }
            }

            return;
        }

        virtual void flush_erase_queue() override
        {
            for(auto const &em : erase_queue) {
                LOG_DEBUG(format("erasing component %s for entity %d") %
                          typeid(CompT).name() %
                          static_cast<int>(em.second->first));
                components.erase(*em.second->second);
                index.erase(em.second);
            }

            erase_queue.clear();
        }
    };

}

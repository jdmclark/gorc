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
        using IdValueT = typename std::underlying_type<IdT>::type;
        using PoolT = pool<CompT, page_size>;
        using IndexT = std::unordered_multimap<IdValueT, CompT*>;

        PoolT components;
        IndexT index;

    public:
        using iterator = typename IndexT::iterator;
        using const_iterator = typename IndexT::const_iterator;

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
            index.emplace(static_cast<IdValueT>(parent), &em);
            return em;
        }

        auto erase(const_iterator const &it)
        {
            components.erase(*it->second);
            return index.erase(it);
        }

        auto erase(const_iterator const &begin, const_iterator const &end)
        {
            for(auto it = begin; it != end; ++it) {
                components.erase(*it->second);
            }

            return index.erase(begin, end);
        }

        auto erase(range<const_iterator> const &rng)
        {
            return erase(rng.begin(), rng.end());
        }

        auto erase(range<iterator> const &rng)
        {
            return erase(rng.begin(), rng.end());
        }

        range<iterator> equal_range(IdT id)
        {
            auto key = static_cast<IdValueT>(id);
            return make_range(index.equal_range(key));
        }

        range<const_iterator> equal_range(IdT id) const
        {
            auto key = static_cast<IdValueT>(id);
            return make_range(index.equal_range(key));
        }

        virtual void erase_equal_range(IdT id) override
        {
            erase(equal_range(id));
        }
    };

}

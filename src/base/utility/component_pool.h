#pragma once

#include "entity_id.h"
#include "pool.h"
#include "range.h"
#include <unordered_map>
#include <utility>
#include <tuple>

namespace gorc {
inline namespace utility {

template <typename CompT, size_t CompPoolPageSize = 128> class component_pool {
    friend class iterator;
    friend class const_iterator;

private:
    class component_container {
    public:
        std::pair<const entity_id, CompT> component_pair;

        template <typename... Args> component_container(entity_id parent, Args&&... args)
            : component_pair(std::piecewise_construct, std::make_tuple(parent), std::forward_as_tuple(args...)) {
            return;
        }
    };

    pool<component_container, CompPoolPageSize> components;
    std::unordered_multimap<int, int> index;

public:

    template <typename... Args> void emplace(entity_id parent, Args&&... args) {
        auto& em = components.emplace(parent, std::forward<Args>(args)...);
        index.emplace(static_cast<int>(parent), em.get_id());
    }

    class iterator {
        friend class component_pool;
    private:
        component_pool& pool;
        std::unordered_multimap<int, int>::iterator inner_iterator;

        iterator(component_pool& pool, const std::unordered_multimap<int, int>::iterator& inner_iterator)
            : pool(pool), inner_iterator(inner_iterator) {
            return;
        }

        inline int get_pool_id() const {
            return inner_iterator->second;
        }

    public:
        using PoolCompT = CompT;

        inline bool operator==(const iterator& it) const {
            return (&pool == &it.pool) && (inner_iterator == it.inner_iterator);
        }

        inline bool operator!=(const iterator& it) const {
            return (&pool != &it.pool) || (inner_iterator != it.inner_iterator);
        }

        inline iterator& operator++() {
            ++inner_iterator;
            return *this;
        }

        inline iterator operator++(int) {
            iterator rv(pool, inner_iterator);
            ++inner_iterator;

            return rv;
        }

        inline std::pair<const entity_id, CompT>& operator*() const {
            return pool.components[inner_iterator->second].component_pair;
        }

        inline std::pair<const entity_id, CompT>* operator->() const {
            return &pool.components[inner_iterator->second].component_pair;
        }
    };

    class const_iterator {
        friend class component_pool;
    private:
        const component_pool& pool;
        std::unordered_multimap<int, int>::const_iterator inner_iterator;

        const_iterator(const component_pool& pool, const std::unordered_multimap<int, int>::const_iterator& inner_iterator)
            : pool(pool), inner_iterator(inner_iterator) {
            return;
        }

        inline int get_pool_id() const {
            return inner_iterator->second;
        }

    public:
        using PoolCompT = CompT;

        inline bool operator==(const const_iterator& it) const {
            return (&pool == &it.pool) && (inner_iterator == it.inner_iterator);
        }

        inline bool operator!=(const const_iterator& it) const {
            return (&pool != &it.pool) || (inner_iterator != it.inner_iterator);
        }

        inline const_iterator& operator++() {
            ++inner_iterator;
            return *this;
        }

        inline const_iterator operator++(int) {
            const_iterator rv(pool, inner_iterator);
            ++inner_iterator;

            return rv;
        }

        inline const std::pair<const entity_id, CompT>& operator*() const {
            return pool.components[inner_iterator->second].component_pair;
        }

        inline const std::pair<const entity_id, CompT>* operator->() const {
            return &pool.components[inner_iterator->second].component_pair;
        }
    };

    iterator begin() {
        return iterator(*this, index.begin());
    }

    const_iterator begin() const {
        return const_iterator(*this, index.begin());
    }

    iterator end() {
        return iterator(*this, index.end());
    }

    const_iterator end() const {
        return const_iterator(*this, index.end());
    }

    range<iterator> find(entity_id parent) {
        auto rng = index.equal_range(static_cast<int>(parent));
        return make_range(iterator(*this, rng.first), iterator(*this, rng.second));
    }

    range<const_iterator> find(entity_id parent) const {
        auto rng = index.equal_range(static_cast<int>(parent));
        return make_range(const_iterator(*this, rng.first), const_iterator(*this, rng.second));
    }

    void erase(const range<iterator>& rng) {
        for(auto it = rng.begin(); it != rng.end(); ++it) {
            components.erase(it.get_pool_id());
        }

        index.erase(rng.begin().inner_iterator, rng.end().inner_iterator);
    }

    void erase(const iterator& it) {
        components.erase(it.get_pool_id());
        index.erase(it.inner_iterator);
    }
};

}
}

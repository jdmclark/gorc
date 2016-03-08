#pragma once

#include "component_pool.hpp"
#include "pool.hpp"
#include <memory>
#include "aspect.hpp"
#include "utility/event_bus.hpp"
#include "libold/base/events/destroyed.hpp"

namespace gorc {
inline namespace utility {

class component_system {
private:
    struct entity { };
    pool<entity, 1024> entities;

public:
    class entity_iterator {
    private:
        pool<entity, 1024>::iterator it;

    public:
        explicit entity_iterator(const pool<entity, 1024>::iterator& it);

        inline entity_id operator*() const {
            return entity_id(it->get_id());
        }

        inline entity_iterator& operator++() {
            ++it;
            return *this;
        }

        inline entity_iterator operator++(int) {
            entity_iterator rv(*this);
            ++it;
            return rv;
        }

        inline bool operator==(const entity_iterator& e) const {
            return it == e.it;
        }

        inline bool operator!=(const entity_iterator& e) const {
            return it != e.it;
        }
    };

    class entity_const_iterator {
    private:
        pool<entity, 1024>::const_iterator it;

    public:
        explicit entity_const_iterator(const pool<entity, 1024>::const_iterator& it);

        inline entity_id operator*() const {
            return entity_id(it->get_id());
        }

        inline entity_const_iterator& operator++() {
            ++it;
            return *this;
        }

        inline entity_const_iterator operator++(int) {
            entity_const_iterator rv(*this);
            ++it;
            return rv;
        }

        inline bool operator==(const entity_const_iterator& e) const {
            return it == e.it;
        }

        inline bool operator!=(const entity_const_iterator& e) const {
            return it != e.it;
        }
    };

private:
    class pool_container {
    public:
        virtual ~pool_container();
        virtual void erase_entity(entity_id id) = 0;
    };

    template <typename CompT> class pool_container_impl : public pool_container {
    public:
        component_pool<CompT> components;

        virtual void erase_entity(entity_id id) override {
            components.erase(components.find(id));
        }
    };

    std::unordered_map<uint32_t, std::unique_ptr<pool_container>> pool_containers;

    template <typename T> inline component_pool<T>& create_and_get_pool() {
        auto it = pool_containers.emplace(static_cast<uint32_t>(T::UID::UID), std::make_unique<pool_container_impl<T>>()).first;
        return reinterpret_cast<pool_container_impl<T>*>(it->second.get())->components;
    }

    template <typename T> inline component_pool<T>& get_pool() {
        auto it = pool_containers.find(static_cast<uint32_t>(T::UID::UID));
        if(it == pool_containers.end()) {
            return create_and_get_pool<T>();
        }

        return reinterpret_cast<pool_container_impl<T>*>(it->second.get())->components;
    }

    std::vector<std::unique_ptr<aspect>> aspects;

public:
    event_bus &bus;

    explicit component_system(event_bus &bus);

    void update(gorc::time time);
    void draw(gorc::time time);

    inline entity_id make_entity() {
        return entity_id(entities.emplace().get_id());
    }

    inline void erase_entity(entity_id id) {
        bus.fire_event(events::destroyed(id));

        for(auto &pool : pool_containers) {
            pool.second->erase_entity(id);
        }

        entities.erase(static_cast<int>(id));
    }

    inline range<entity_iterator> all_entities() {
        return make_range(entity_iterator(entities.begin()), entity_iterator(entities.end()));
    }

    inline range<entity_const_iterator> all_entities() const {
        return make_range(entity_const_iterator(entities.begin()), entity_const_iterator(entities.end()));
    }

    template <typename T> inline void register_component_type() {
        get_pool<T>();
    }

    template <typename T, typename... ArgT>
    inline T& emplace_component(entity_id parent, ArgT&&... args) {
        return get_pool<T>().emplace(parent, std::forward<ArgT>(args)...);
    }

    template <typename T> inline range<typename component_pool<T>::iterator> find_component(entity_id parent) {
        return get_pool<T>().find(parent);
    }

    template <typename T> inline range<typename component_pool<T>::iterator> all_components() {
        auto& p = get_pool<T>();
        return make_range(p.begin(), p.end());
    }

    template <typename IteratorT> inline void erase_component(const IteratorT& it) {
        get_pool<typename IteratorT::PoolCompT>().erase(it);
    }

    template <typename IteratorT> inline void erase_components(range<IteratorT> rng) {
        for(auto it = rng.begin(); it != rng.end();) {
            erase_component(it++);
        }
    }

    template <typename IteratorT, typename PredT>
    inline void erase_components(range<IteratorT> rng, PredT pred) {
        for(auto it = rng.begin(); it != rng.end();) {
            if(pred(it->second)) {
                erase_component(it++);
            }
            else {
                ++it;
            }
        }
    }

    template <typename T, typename... ArgT> inline void emplace_aspect(ArgT&&... args) {
        aspects.push_back(std::make_unique<T>(*this, std::forward<ArgT>(args)...));
    }
};

}
}

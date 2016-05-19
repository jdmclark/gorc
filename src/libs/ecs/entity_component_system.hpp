#pragma once

#include "sequential_entity_generator.hpp"
#include "component_relational_mapping.hpp"
#include "component_registry.hpp"
#include "aspect.hpp"
#include "entity_destroyed.hpp"
#include "utility/event_bus.hpp"
#include "utility/maybe.hpp"
#include "utility/service_registry.hpp"
#include "log/log.hpp"

#include <vector>
#include <memory>

namespace gorc {

    template <typename IdT>
    class entity_component_system {
    private:
        sequential_entity_generator<IdT> entities;
        component_relational_mapping<IdT> components;
        std::vector<std::unique_ptr<aspect>> aspects;

    public:
        event_bus &bus;

        explicit entity_component_system(service_registry const &services)
            : bus(services.get<event_bus>())
        {
            services.get<component_registry<IdT>>().register_component_types(components);
            return;
        }

        auto emplace_entity()
        {
            return entities.emplace();
        }

        void erase_entity(IdT entity)
        {
            LOG_DEBUG(format("erased entity %d") % static_cast<int>(entity));
            bus.fire_event(entity_destroyed<IdT>(entity));
            components.erase_equal_range(entity);
            entities.erase(entity);
        }

        template <typename CompT, typename ...ArgT>
        auto& emplace_component(IdT entity, ArgT &&...args)
        {
            return components.template emplace<CompT>(entity, std::forward<ArgT>(args)...);
        }

        template <typename CompT>
        auto all_components()
        {
            return components.component_relational_mapping<IdT>::template range<CompT>();
        }

        template <typename CompT>
        auto find_component(IdT entity)
        {
            return components.template equal_range<CompT>(entity);
        }

        template <typename CompT, typename ...ArgT>
        auto& get_unique_component(IdT entity, ArgT &&...args)
        {
            for(auto &comp : find_component<CompT>(entity)) {
                return *comp.second;
            }

            return emplace_component<CompT>(entity, std::forward<ArgT>(args)...);
        }

        template <typename CompT>
        maybe<CompT*> maybe_get_unique_component(IdT entity)
        {
            for(auto &comp : find_component<CompT>(entity)) {
                return comp.second;
            }

            return nothing;
        }

        template <typename CompT>
        void erase_components(IdT entity)
        {
            auto rng = components.template equal_range<CompT>(entity);
            components.template erase(rng.begin(), rng.end());
        }

        template <typename CompT, typename PredT>
        void erase_component_if(PredT pred)
        {
            components.template erase_if<CompT>(pred);
        }

        template <typename T, typename ...ArgT>
        void emplace_aspect(ArgT &&...args)
        {
            aspects.push_back(std::make_unique<T>(*this, std::forward<ArgT>(args)...));
        }

        void update(time_delta dt)
        {
            for(auto &aspect : aspects) {
                aspect->update(dt);
            }

            components.flush_erase_queue();
            entities.flush_erase_queue();
            return;
        }
    };


}

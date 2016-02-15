#pragma once

#include "aspect.hpp"
#include "content/id.hpp"
#include "entity_component_system.hpp"
#include <type_traits>

namespace gorc {

    namespace detail {

        template <typename IdT, typename ...CompT>
        struct inner_join_aspect_apply_args;

        template <typename IdT>
        struct inner_join_aspect_apply_args<IdT> {
            template <typename FnT, typename ...ArgT>
            void apply(entity_component_system<IdT> &,
                       time_delta dt,
                       IdT entity,
                       FnT const &fn,
                       ArgT &...args) const
            {
                fn(dt, entity, args...);
            }
        };

        template <typename IdT, typename HeadCompT, typename ...CompT>
        struct inner_join_aspect_apply_args<IdT, HeadCompT, CompT...> {
            template <typename FnT, typename ...ArgT>
            void apply(entity_component_system<IdT> &ecs,
                       time_delta dt,
                       IdT entity,
                       FnT const &fn,
                       ArgT &...args) const
            {
                for(auto &comp : ecs.template component_equal_range<HeadCompT>(entity)) {
                    inner_join_aspect_apply_args<IdT, CompT...>()
                        .apply(ecs, dt, entity, fn, args..., *comp.second);
                }
            }
        };

    }

    template <typename IdT, typename HeadCompT, typename ...CompT>
    class inner_join_aspect : public aspect {
    private:
        entity_component_system<IdT> &ecs;

    public:
        inner_join_aspect(entity_component_system<IdT> &ecs)
            : ecs(ecs)
        {
            return;
        }

        virtual void update(time_delta dt) override
        {
            for(auto &comp : ecs.template component_range<HeadCompT>()) {
                auto fn = [this](time_delta dt, IdT entity, HeadCompT &head_comp, CompT &...comp)
                    {
                        update(dt, entity, head_comp, comp...);
                    };

                detail::inner_join_aspect_apply_args<IdT, CompT...>()
                    .apply(ecs, dt, IdT(comp.first), fn, *comp.second);
            }
        }

        virtual void update(time_delta, IdT, HeadCompT&, CompT& ...) = 0;
    };
}

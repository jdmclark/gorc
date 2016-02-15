#pragma once

#include "sequential_entity_generator.hpp"
#include "component_relational_mapping.hpp"
#include "aspect.hpp"

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
        auto emplace_entity()
        {
            return entities.emplace();
        }

        template <typename CompT, typename ...ArgT>
        auto& emplace_component(IdT entity, ArgT &&...args)
        {
            return components.template emplace<CompT>(entity, std::forward<ArgT>(args)...);
        }

        template <typename CompT>
        auto component_range()
        {
            return components.component_relational_mapping<IdT>::template range<CompT>();
        }

        template <typename CompT>
        auto component_equal_range(IdT entity)
        {
            return components.template equal_range<CompT>(entity);
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

            return;
        }
    };


}

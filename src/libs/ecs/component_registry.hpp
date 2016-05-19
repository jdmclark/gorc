#pragma once

#include "component_relational_mapping.hpp"
#include "utility/uid.hpp"
#include "log/log.hpp"

#include <unordered_map>
#include <memory>
#include <typeinfo>
#include <typeindex>

namespace gorc {

    template <typename IdT>
    class component_registry {
    private:
        using crm = component_relational_mapping<IdT>;

        class abstract_factory {
        public:
            virtual void register_component_type(crm &) const = 0;
        };

        template <typename CompT>
        class factory : public abstract_factory {
        public:
            virtual void register_component_type(crm &c) const override
            {
                c.template register_component_type<CompT>();
            }
        };

        std::unordered_map<uint32_t, std::unique_ptr<abstract_factory>> factories;

    public:
        template <typename T>
        void register_component_type()
        {
            auto value = std::make_unique<factory<T>>();
            auto res = factories.emplace(uid_of<T>(), std::move(value));
            if(!res.second) {
                LOG_FATAL(format("component type with uid %d is already registered") %
                          uid_of<T>());
            }
        }

        void register_component_types(crm &c) const
        {
            for(auto const &factory : factories) {
                factory.second->register_component_type(c);
            }
        }
    };

}

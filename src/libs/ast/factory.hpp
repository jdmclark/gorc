#pragma once

#include "node.hpp"
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace gorc {

    class ast_factory {
    private:
        class factory_factory {
        public:
            virtual ~factory_factory();
        };

        template <typename NodeT>
        class factory_factory_impl : public factory_factory {
        private:
            std::vector<std::unique_ptr<NodeT>> nodes;

        public:
            template <typename ...ArgT>
            NodeT* emplace(ArgT &&...args)
            {
                nodes.emplace_back(std::make_unique<NodeT>(std::forward<ArgT>(args)...));
                return nodes.back().get();
            }
        };

        std::unordered_map<std::type_index, std::unique_ptr<factory_factory>> factories;

        template <typename NodeT>
        factory_factory_impl<NodeT>& get_factory()
        {
            auto it = factories.find(typeid(NodeT));
            if(it == factories.end()) {
                it = factories.emplace(std::type_index(typeid(NodeT)),
                                       std::make_unique<factory_factory_impl<NodeT>>()).first;
            }

            return *reinterpret_cast<factory_factory_impl<NodeT>*>(it->second.get());
        }

    public:
        template <typename NodeT, typename ...ArgT>
        NodeT* make(ArgT &&...args)
        {
            return get_factory<NodeT>().emplace(std::forward<ArgT>(args)...);
        }

        template <typename VariantT, typename NodeT, typename ...ArgT>
        VariantT* make_var(ArgT &&...args)
        {
            return get_factory<VariantT>().emplace(make<NodeT>(std::forward<ArgT>(args)...));
        }
    };

}

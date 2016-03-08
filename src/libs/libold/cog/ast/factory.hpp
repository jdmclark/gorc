#pragma once

#include "node.hpp"
#include <memory>

namespace gorc {
namespace cog {
namespace ast {

class factory {
private:
    std::vector<std::unique_ptr<node>> nodes;

    template <typename T>
    class value_storage_node : public node {
    public:
        T value;

        value_storage_node()
            : node(diagnostic_context_location())
        {
            return;
        }

        explicit value_storage_node(T const &value)
            : node(diagnostic_context_location())
            , value(value)
        {
            return;
        }
    };

public:
    template <typename T> T* store_value(const T& value) {
        auto val = std::make_unique<value_storage_node<T>>(value);
        auto* val_ptr = val.get();
        nodes.push_back(std::move(val));
        return &val_ptr->value;
    }

    template <typename T> std::vector<T*>* make_list() {
        auto val = std::make_unique<value_storage_node<std::vector<T*>>>();
        auto* val_ptr = val.get();
        nodes.push_back(std::move(val));
        return &val_ptr->value;
    }

    template <typename T> std::vector<T*>* make_list(T* initial_value) {
        auto* val_ptr = make_list<T>();
        val_ptr->push_back(initial_value);
        return val_ptr;
    }

    template <typename T, typename... V> T* make(V&&... args) {
        auto val = std::make_unique<T>(std::forward<V>(args)...);
        auto* val_ptr = val.get();
        nodes.push_back(std::move(val));
        return val_ptr;
    }

    template <typename VarT, typename T, typename ...V> VarT* make_var(V &&...args) {
        auto val = make<T>(std::forward<V>(args)...);
        return make<VarT>(val);
    }
};
}
}
}

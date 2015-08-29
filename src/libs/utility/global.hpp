#pragma once

#include <memory>
#include <vector>
#include <typeindex>
#include "uncopyable.hpp"

namespace gorc {

    class abstract_global_factory;
    template <typename GlobalT> class global_factory;

    class global : private uncopyable {
    private:
        std::vector<std::shared_ptr<global>> reserved_global_dependencies;

        std::shared_ptr<global> reserve_global_dependency_with_factory(
                std::type_index tid,
                abstract_global_factory const &factory);

    protected:
        template <typename GlobalT>
        std::shared_ptr<GlobalT> reserve_global_dependency()
        {
            auto ptr = reserve_global_dependency_with_factory(typeid(GlobalT),
                                                              global_factory<GlobalT>());
            return std::dynamic_pointer_cast<GlobalT>(ptr);
        }

    public:
        virtual ~global();
    };

    class abstract_global_factory {
    public:
        virtual ~abstract_global_factory();

        virtual std::shared_ptr<global> make_global() const = 0;
    };

    template <typename GlobalT>
    class global_factory : public abstract_global_factory {
    public:
        virtual std::shared_ptr<global> make_global() const override
        {
            return std::shared_ptr<global>(new GlobalT());
        }
    };

    std::shared_ptr<global> get_or_create_global(std::type_index tid,
                                                 abstract_global_factory const &factory);

    template <typename GlobalT>
    std::shared_ptr<GlobalT> get_global()
    {
        auto g = get_or_create_global(typeid(GlobalT), global_factory<GlobalT>());
        return std::dynamic_pointer_cast<GlobalT>(g);
    }
}

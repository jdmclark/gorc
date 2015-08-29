#pragma once

#include <memory>
#include <vector>
#include <typeindex>
#include "uncopyable.hpp"

namespace gorc {

    class abstract_local_factory;
    template <typename LocalT> class local_factory;

    class local : private uncopyable {
    public:
        virtual ~local();
    };

    class abstract_local_factory {
    public:
        virtual ~abstract_local_factory();

        virtual std::shared_ptr<local> make_local() const = 0;
    };

    template <typename LocalT>
    class local_factory : public abstract_local_factory {
    public:
        virtual std::shared_ptr<local> make_local() const override
        {
            return std::shared_ptr<local>(new LocalT());
        }
    };

    std::shared_ptr<local> get_or_create_local(std::type_index tid,
                                               abstract_local_factory const &factory);

    template <typename LocalT>
    std::shared_ptr<LocalT> get_local()
    {
        auto g = get_or_create_local(typeid(LocalT), local_factory<LocalT>());
        return std::dynamic_pointer_cast<LocalT>(g);
    }

}

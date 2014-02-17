#pragma once

#include "cog/vm/type.h"
#include "cog/vm/value.h"
#include "verb_binder.h"
#include <stack>
#include <functional>
#include <boost/function_types/result_type.hpp>
#include <boost/function_types/function_arity.hpp>

namespace gorc {
namespace cog {
namespace verbs {

template <typename T> struct helper_get_vm_type { };

template <> struct helper_get_vm_type<void> {
    static const vm::type type = vm::type::nothing;
};

template <> struct helper_get_vm_type<int> {
    static const vm::type type = vm::type::integer;
};

template <> struct helper_get_vm_type<float> {
    static const vm::type type = vm::type::floating;
};

template <> struct helper_get_vm_type<bool> {
    static const vm::type type = vm::type::boolean;
};

template <> struct helper_get_vm_type<vector<3>> {
    static const vm::type type = vm::type::vector;
};

template <> struct helper_get_vm_type<vm::value> {
    static const vm::type type = vm::type::dynamic;
};

class base_verb {
private:
    const size_t parameterCount;
    const vm::type returnType;

public:
    base_verb(vm::type returnType, size_t parameterCount);
    virtual ~base_verb();

    inline vm::type return_type() const {
        return returnType;
    }

    inline size_t parameter_count() const {
        return parameterCount;
    }

    virtual vm::value invoke(std::stack<vm::value>& stack, void* system) const = 0;
};

template <typename ResultType, int Arity, typename F> class verb : public base_verb {
private:
    F functor;

public:
    verb(F functor) : base_verb(helper_get_vm_type<ResultType>::type, Arity), functor(functor) {
        return;
    }

    vm::value invoke(std::stack<vm::value>& stack, void* system) const {
        verb_binder<ResultType, Arity> binder;
        return binder.invoke(stack, functor);
    }
};

template <typename ResultType, int Arity, typename SystemRefT, typename F> class system_verb : public base_verb {
private:
    F functor;

public:
    system_verb(F functor) : base_verb(helper_get_vm_type<ResultType>::type, Arity), functor(functor) {
        return;
    }

    vm::value invoke(std::stack<vm::value>& stack, void* system) const {
        system_verb_binder<ResultType, Arity, SystemRefT> binder;
        return binder.invoke(stack, *reinterpret_cast<SystemRefT*>(system), functor);
    }
};

}
}
}

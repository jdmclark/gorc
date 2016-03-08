#pragma once

#include "libold/cog/vm/value.hpp"
#include <stack>

namespace gorc {
namespace cog {
namespace verbs {

template <typename T, size_t n>
class verb_binder {
public:
    template <typename FnT, typename ...ArgT>
    vm::value invoke(std::stack<vm::value>& stk, FnT fn, ArgT ...args) {
        auto v0 = stk.top();
        stk.pop();
        return verb_binder<T, n - 1>().invoke(stk, fn, v0, args...);
    }
};

template <typename T>
class verb_binder<T, 0> {
public:
    template <typename FnT, typename ...ArgT>
    vm::value invoke(std::stack<vm::value>&, FnT fn, ArgT ...args) {
        return vm::value(fn(args...));
    }
};

template < >
class verb_binder<void, 0> {
public:
    template <typename FnT, typename ...ArgT>
    vm::value invoke(std::stack<vm::value>&, FnT fn, ArgT ...args) {
        fn(args...);
        return vm::value();
    }
};

template <typename T, size_t n, typename SystemRefT>
class system_verb_binder {
public:
    template <typename FnT, typename ...ArgT>
    vm::value invoke(std::stack<vm::value>& stk, SystemRefT &sys, FnT fn, ArgT ...args) {
        auto v0 = stk.top();
        stk.pop();
        return system_verb_binder<T, n - 1, SystemRefT>().invoke(stk, sys, fn, v0, args...);
    }
};

template <typename T, typename SystemRefT>
class system_verb_binder<T, 0, SystemRefT> {
public:
    template <typename FnT, typename ...ArgT>
    vm::value invoke(std::stack<vm::value>&, SystemRefT &sys, FnT fn, ArgT ...args) {
        return vm::value(fn(args..., sys));
    }
};

template <typename SystemRefT>
class system_verb_binder<void, 0, SystemRefT> {
public:
    template <typename FnT, typename ...ArgT>
    vm::value invoke(std::stack<vm::value>&, SystemRefT &sys, FnT fn, ArgT ...args) {
        fn(args..., sys);
        return vm::value();
    }
};

}
}
}

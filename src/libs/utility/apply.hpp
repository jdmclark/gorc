#pragma once

#include <tuple>

namespace gorc {

    using std::tuple_size;
    using std::get;

    template <typename FnT, typename TupleT, size_t n>
    struct apply_packer {
        template <typename... EmT>
        auto operator()(FnT &fn, TupleT const &t, EmT const & ...em) const
            -> decltype(apply_packer<FnT, TupleT, n - 1>()(fn, t, get<n - 1>(t), em...))
        {
            return apply_packer<FnT, TupleT, n - 1>()(fn, t, get<n - 1>(t), em...);
        }
    };

    template <typename FnT, typename TupleT>
    struct apply_packer<FnT, TupleT, 0> {
        template <typename... EmT>
        auto operator()(FnT &fn, TupleT const &, EmT const & ...em) const
            -> decltype(fn(em...))
        {
            return fn(em...);
        }
    };

    template <typename FnT, typename TupleT>
    auto apply(FnT &fn, TupleT const &t)
        -> decltype(apply_packer<FnT, TupleT, tuple_size<TupleT>::value>()(fn, t))
    {
        return apply_packer<FnT, TupleT, tuple_size<TupleT>::value>()(fn, t);
    }

}

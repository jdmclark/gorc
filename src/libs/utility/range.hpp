#pragma once

#include <tuple>

namespace gorc {

    using std::get;
    using std::tuple_size;

    template <typename It>
    class range {
    private:
        It first, last;

    public:
        range(It const &first, It const &last)
            : first(first)
            , last(last)
        {
            return;
        }

        It const& begin() const
        {
            return first;
        }

        It const& end() const
        {
            return last;
        }

        inline bool empty() const
        {
            return first == last;
        }
    };

    template <typename It>
    range<It> make_range(It const &first, It const &last)
    {
        return range<It>(first, last);
    }

    template <typename TupleT>
    auto make_range(TupleT const &tuple)
        -> decltype(make_range(get<0>(tuple), get<1>(tuple)))
    {
        static_assert(tuple_size<TupleT>::value == 2, "tuple must be a pair");
        return make_range(get<0>(tuple), get<1>(tuple));
    }

    template <typename RangeT>
    auto make_range(RangeT &rng)
        -> decltype(make_range(rng.begin(), rng.end()))
    {
        return make_range(rng.begin(), rng.end());
    }

    template <typename ContainerT>
    auto make_reverse_range(ContainerT &rng)
        -> decltype(make_range(rng.rbegin(), rng.rend()))
    {
        return make_range(rng.rbegin(), rng.rend());
    }

}

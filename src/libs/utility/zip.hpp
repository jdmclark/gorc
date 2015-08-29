#pragma once

#include "range.hpp"
#include <tuple>
#include <iterator>

namespace gorc {

    template <typename TupleT, size_t n>
    struct zip_iterator_dereference_packer {
        template <typename... EmT>
        auto operator()(TupleT const &a, EmT& ...ems) const
            -> decltype(zip_iterator_dereference_packer<TupleT, n - 1>()(a,
                                                                         *std::get<n - 1>(a),
                                                                         ems...))
        {
            return zip_iterator_dereference_packer<TupleT, n - 1>()(a, *std::get<n - 1>(a), ems...);
        }
    };

    template <typename TupleT>
    struct zip_iterator_dereference_packer<TupleT, 0> {
        template <typename... EmT>
        auto operator()(TupleT const &, EmT& ...ems) const
            -> decltype(std::forward_as_tuple(ems...))
        {
            return std::forward_as_tuple(ems...);
        }
    };

    template <typename TupleT, size_t n>
    struct zip_iterator_inner_comparator {
        bool operator()(TupleT const &a, TupleT const &b) const
        {
            using namespace std;
            return get<n - 1>(a) == get<n - 1>(b) ||
                   zip_iterator_inner_comparator<TupleT, n - 1>()(a, b);
        }
    };

    template <typename TupleT>
    struct zip_iterator_inner_comparator<TupleT, 0> {
        bool operator()(TupleT const &, TupleT const &) const
        {
            return false;
        }
    };

    template <typename TupleT, size_t n>
    struct zip_iterator_comparator {
        bool operator()(TupleT const &a, TupleT const &b) const
        {
            return zip_iterator_inner_comparator<TupleT, n>()(a, b);
        }
    };

    template <typename TupleT>
    struct zip_iterator_comparator<TupleT, 0> {
        bool operator()(TupleT const &, TupleT const &) const
        {
            // Empty zip iterators are always equal
            return true;
        }
    };

    template <typename TupleT, size_t n>
    struct zip_iterator_incrementor {
        void operator()(TupleT &a) const
        {
            using namespace std;
            ++get<n - 1>(a);
            zip_iterator_incrementor<TupleT, n - 1>()(a);
        }
    };

    template <typename TupleT>
    struct zip_iterator_incrementor<TupleT, 0> {
        void operator()(TupleT &) const
        {
            return;
        }
    };

    template <typename... EmT>
    class zip_iterator {
    private:
        using TupleT = std::tuple<EmT...>;
        TupleT values;

    public:
        zip_iterator(TupleT const &values)
            : values(values)
        {
            return;
        }

        auto operator*() const
            -> decltype(zip_iterator_dereference_packer<TupleT,
                                                        std::tuple_size<TupleT>::value>()(values))
        {
            return zip_iterator_dereference_packer<TupleT, std::tuple_size<TupleT>::value>()(values);
        }

        TupleT const* operator->() const
        {
            return &values;
        }

        bool operator==(zip_iterator const &it) const
        {
            using namespace std;
            return zip_iterator_comparator<TupleT, tuple_size<TupleT>::value>()(values, it.values);
        }

        bool operator!=(zip_iterator const &it) const
        {
            return !(*this == it);
        }

        zip_iterator& operator++()
        {
            using namespace std;
            zip_iterator_incrementor<TupleT, tuple_size<TupleT>::value>()(values);
            return *this;
        }

        zip_iterator operator++(int)
        {
            zip_iterator rv = *this;

            using namespace std;
            zip_iterator_incrementor<TupleT, tuple_size<TupleT>::value>()(values);
            return rv;
        }
    };

    template <typename... EmT>
    zip_iterator<EmT...> make_zip_iterator(EmT&&... em)
    {
        return zip_iterator<EmT...>(std::make_tuple(em...));
    }

    template <typename... RangeT>
    auto zip(RangeT &... rng)
        -> decltype(make_range(make_zip_iterator(rng.begin()...),
                               make_zip_iterator(rng.end()...)))
    {
        return make_range(make_zip_iterator(rng.begin()...),
                          make_zip_iterator(rng.end()...));
    }

}

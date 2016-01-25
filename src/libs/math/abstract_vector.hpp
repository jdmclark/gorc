#pragma once

#include <cstddef>
#include <type_traits>
#include <utility>
#include <algorithm>

#include "almost_equal.hpp"
#include "utility/constructor_tag.hpp"

namespace gorc {

    template <size_t n, typename F, typename Tag>
    class abstract_vector {
    public:
        F data[n];

        constexpr abstract_vector()
            : data{ F(0) }
        {
        }

        explicit abstract_vector(uninit_constructor_tag)
        {
        }

        template <typename ...ArgT, class = typename std::enable_if<sizeof...(ArgT) == n>::type>
        constexpr explicit abstract_vector(ArgT ...v)
            : data { std::forward<ArgT>(v)... }
        {
        }

        F* begin()
        {
            return data;
        }

        F const* begin() const
        {
            return data;
        }

        F* end()
        {
            return data + n;
        }

        F const* end() const
        {
            return data + n;
        }

        template <typename G>
        operator abstract_vector<n, G, Tag>() const
        {
            abstract_vector<n, G, Tag> rv(uninit_constructor);
            std::transform(begin(), end(), rv.begin(), [](F em) { return static_cast<G>(em); });
            return rv;
        }
    };

    template <typename Tag, typename F, typename ...G>
    constexpr abstract_vector<sizeof...(G) + 1, F, Tag> make_abstract_vector(F v0, G ...v)
    {
        return abstract_vector<sizeof...(G) + 1, F, Tag>(v0, v...);
    }

    template <size_t n, typename F, typename Tag>
    constexpr abstract_vector<n, F, Tag> make_fill_abstract_vector(F c)
    {
        auto rv = abstract_vector<n, F, Tag>(uninit_constructor);
        std::fill(rv.begin(), rv.end(), c);
        return rv;
    }

    template <size_t idx, size_t n, typename F, typename Tag>
    constexpr F& get(abstract_vector<n, F, Tag> &vec)
    {
        static_assert(idx < n, "index out of bounds");
        return vec.data[idx];
    }

    template <size_t idx, size_t n, typename F, typename Tag>
    constexpr F get(abstract_vector<n, F, Tag> const &vec)
    {
        static_assert(idx < n, "index out of bounds");
        return vec.data[idx];
    }

    template <size_t n, typename F, typename Tag>
    bool almost_equal(abstract_vector<n, F, Tag> const &v,
                      abstract_vector<n, F, Tag> const &w)
    {
        for(size_t i = 0; i < n; ++i) {
            if(!almost_equal(v.data[i], w.data[i])) {
                return false;
            }
        }

        return true;
    }

    template <size_t n, typename F, typename Tag>
    bool operator==(abstract_vector<n, F, Tag> const &v,
                    abstract_vector<n, F, Tag> const &w)
    {
        return std::equal(v.begin(), v.end(), w.begin());
    }

    template <size_t n, typename F, typename Tag>
    bool operator!=(abstract_vector<n, F, Tag> const &v,
                    abstract_vector<n, F, Tag> const &w)
    {
        return std::equal(v.begin(), v.end(), w.begin(), std::not_equal_to<decltype(v)>());
    }

    template <size_t n, typename F, typename TagF, typename TagG>
    auto operator+(abstract_vector<n, F, TagF> const &v,
                   abstract_vector<n, F, TagG> const &w)
    {
        abstract_vector<n, F, decltype(TagF() + TagG())> rv(uninit_constructor);
        std::transform(v.begin(), v.end(), w.begin(), rv.begin(), std::plus<F>());
        return rv;
    }

    template <size_t n, typename F, typename TagF, typename TagG>
    auto operator-(abstract_vector<n, F, TagF> const &v,
                   abstract_vector<n, F, TagG> const &w)
    {
        abstract_vector<n, F, decltype(TagF() - TagG())> rv(uninit_constructor);
        std::transform(v.begin(), v.end(), w.begin(), rv.begin(), std::minus<F>());
        return rv;
    }

    template <size_t n, typename F, typename Tag>
    auto operator*(abstract_vector<n, F, Tag> const &v, F c)
    {
        abstract_vector<n, F, Tag> rv(uninit_constructor);
        std::transform(v.begin(), v.end(), rv.begin(), [c](F x) { return x * c; });
        return rv;
    }

    template <size_t n, typename F, typename Tag>
    auto operator/(abstract_vector<n, F, Tag> const &v, F c)
    {
        abstract_vector<n, F, Tag> rv(uninit_constructor);
        std::transform(v.begin(), v.end(), rv.begin(), [c](F x) { return x / c; });
        return rv;
    }

}

namespace std {

    // Legal specializations under the C++11 standard

    template <size_t n, typename F, typename Tag>
    class tuple_size<::gorc::abstract_vector<n, F, Tag>>
        : public std::integral_constant<std::size_t, n> { };
}

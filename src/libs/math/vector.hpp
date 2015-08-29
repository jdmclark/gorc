#pragma once

#include <cstddef>
#include <type_traits>
#include <utility>

#include "almost_equal.hpp"
#include "utility/constructor_tag.hpp"

namespace gorc {

    struct direction_vector_tag { };

    template <size_t n, typename F = float, typename Tag = direction_vector_tag>
    class vector {
    public:
        F data[n];

        constexpr vector()
            : data{ F(0) }
        {
        }

        explicit vector(uninit_constructor_tag)
        {
        }

        template <typename ...ArgT, class = typename std::enable_if<sizeof...(ArgT) == n>::type>
        constexpr explicit vector(ArgT ...v)
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
    };

    template <typename F, typename ...G, typename Tag = direction_vector_tag>
    constexpr vector<sizeof...(G) + 1, F, Tag> make_vector(F v0, G ...v)
    {
        return vector<sizeof...(G) + 1, F, Tag>(v0, v...);
    }

    template <size_t idx, size_t n, typename F, typename Tag>
    constexpr F& get(vector<n, F, Tag> &vec)
    {
        static_assert(idx < n, "index out of bounds");
        return vec.data[idx];
    }

    template <size_t idx, size_t n, typename F, typename Tag>
    constexpr F get(vector<n, F, Tag> const &vec)
    {
        static_assert(idx < n, "index out of bounds");
        return vec.data[idx];
    }

    template <size_t n, typename F, typename Tag>
    bool almost_equal(vector<n, F, Tag> const &v, vector<n, F, Tag> const &w)
    {
        for(size_t i = 0; i < n; ++i) {
            if(!almost_equal(v.data[i], w.data[i])) {
                return false;
            }
        }

        return true;
    }
}

namespace std {

    // Legal specializations under the C++11 standard

    template <size_t n, typename F, typename Tag>
    class tuple_size<::gorc::vector<n, F, Tag>>
        : public std::integral_constant<std::size_t, n> { };
}

#pragma once

#include <cstddef>
#include <type_traits>
#include <utility>
#include <algorithm>

#include "almost_equal.hpp"
#include "utility/constructor_tag.hpp"
#include "utility/apply.hpp"

#include "text/json_input_stream.hpp"
#include "text/json_output_stream.hpp"
#include "io/binary_input_stream.hpp"
#include "io/binary_output_stream.hpp"

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

        abstract_vector(deserialization_constructor_tag, binary_input_stream &bis)
        {
            for(auto &em : data) {
                em = binary_deserialize<F>(bis);
            }
        }

        void binary_serialize_object(binary_output_stream &bos) const
        {
            for(auto const &em : data) {
                binary_serialize(bos, em);
            }
        }

        abstract_vector(deserialization_constructor_tag, json_input_stream &jis)
        {
            json_deserialize_array(jis, data.begin(), data.end());
        }

        void json_serialize_object(json_output_stream &jos) const
        {
            json_serialize_array(jos, data);
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

        template <typename TagG,
                  typename = std::enable_if<std::is_same<decltype(Tag() + TagG()), Tag>::value>>
        abstract_vector& operator+=(abstract_vector<n, F, TagG> const &w)
        {
            auto out_it = begin();
            auto jt = w.begin();
            for( ; out_it != end() && jt != w.end(); ++out_it, ++jt) {
                *out_it += *jt;
            }

            return *this;
        }

        template <typename TagG,
                  typename = std::enable_if<std::is_same<decltype(Tag() - TagG()), Tag>::value>>
        abstract_vector& operator-=(abstract_vector<n, F, TagG> const &w)
        {
            auto out_it = begin();
            auto jt = w.begin();
            for( ; out_it != end() && jt != w.end(); ++out_it, ++jt) {
                *out_it -= *jt;
            }

            return *this;
        }

        abstract_vector& operator*=(F c)
        {
            for(auto &em : data) {
                em *= c;
            }

            return *this;
        }

        abstract_vector& operator/=(F c)
        {
            for(auto &em : data) {
                em /= c;
            }

            return *this;
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

    template <size_t n, typename F, typename Tag, size_t m>
    constexpr abstract_vector<n, F, Tag> extend_vector(abstract_vector<m, F, Tag> const &v, F c)
    {
        auto rv = abstract_vector<n, F, Tag>(uninit_constructor);
        auto out_it = rv.begin();
        for(auto it = v.begin(); it != v.end() && out_it != rv.end(); ++it, ++out_it) {
            *out_it = *it;
        }

        for( ; out_it != rv.end(); ++out_it) {
            *out_it = c;
        }

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
        return std::equal(v.begin(), v.end(), w.begin(), std::not_equal_to<F>());
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
    auto operator*(F c, abstract_vector<n, F, Tag> const &v)
    {
        abstract_vector<n, F, Tag> rv(uninit_constructor);
        std::transform(v.begin(), v.end(), rv.begin(), [c](F x) { return c * x; });
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

    template <size_t n, typename F, typename Tag>
    auto operator-(abstract_vector<n, F, Tag> const &v)
    {
        abstract_vector<n, F, Tag> rv(uninit_constructor);
        std::transform(v.begin(), v.end(), rv.begin(), std::negate<F>());
        return rv;
    }

    template <size_t n, typename F, typename Tag>
    auto length_squared(abstract_vector<n, F, Tag> const &v)
    {
        F c = F(0);
        for(auto const &em : v) {
            c += (em * em);
        }

        return c;
    }

    template <size_t n, typename F, typename Tag>
    auto length(abstract_vector<n, F, Tag> const &v)
    {
        return static_cast<F>(std::sqrt(length_squared(v)));
    }

    template <size_t n, typename F, typename Tag>
    auto dot(abstract_vector<n, F, Tag> const &v, abstract_vector<n, F, Tag> const &w)
    {
        F c = F(0);
        auto it = v.begin();
        auto jt = w.begin();
        for( ; it != v.end() && jt != w.end(); ++it, ++jt) {
            c += (*it * *jt);
        }

        return c;
    }

    template <size_t n, typename F, typename Tag>
    auto normalize(abstract_vector<n, F, Tag> const &v)
    {
        return v / length(v);
    }

    template <typename F, typename Tag>
    auto cross(abstract_vector<3, F, Tag> const &v, abstract_vector<3, F, Tag> const &w)
    {
        auto rv = abstract_vector<3, F, Tag>(uninit_constructor);
        get<0>(rv) = get<1>(v) * get<2>(w) - get<2>(v) * get<1>(w);
        get<1>(rv) = get<2>(v) * get<0>(w) - get<0>(v) * get<2>(w);
        get<2>(rv) = get<0>(v) * get<1>(w) - get<1>(v) * get<0>(w);
        return rv;
    }

}

namespace std {

    // Legal specializations under the C++11 standard

    template <size_t n, typename F, typename Tag>
    class tuple_size<::gorc::abstract_vector<n, F, Tag>>
        : public std::integral_constant<std::size_t, n> { };

}

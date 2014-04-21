#pragma once

#include <array>
#include <initializer_list>
#include <utility>
#include <algorithm>
#include <iostream>

namespace gorc {
inline namespace math {

template <size_t n, typename F = float> class vector {
    template <size_t m, typename G> friend vector<m, G> operator*(G, const vector<m, G>&);
    template <unsigned int idx, size_t m, typename G> friend G& get(vector<m, G>&);
    template <unsigned int idx, size_t m, typename G> friend G get(const vector<m, G>&);
    template <size_t m, typename G> friend vector<m, G> make_zero_vector();
    template <size_t m, typename G> friend vector<m, G> make_fill_vector(G);

private:
    std::array<F, n> data;

public:
    constexpr vector() { }

    template <typename... G, class = typename std::enable_if<sizeof...(G) == n>::type> constexpr explicit vector(G... v)
        : data {{ std::forward<G>(v)... }} { }

    inline auto begin() const -> decltype(data.begin()) {
        return data.begin();
    }

    inline auto begin() -> decltype(data.begin()) {
        return data.begin();
    }

    inline auto end() const -> decltype(data.end()) {
        return data.end();
    }

    inline auto end() -> decltype(data.end()) {
        return data.end();
    }

    inline F& operator[](int index) {
        return data[index];
    }

    inline const F& operator[](int index) const {
        return data[index];
    }

    inline vector operator+(const vector& v) const {
        vector rv;
        std::transform(begin(), end(), v.begin(), rv.begin(), std::plus<F>());
        return rv;
    }

    inline void operator+=(const vector& v) {
        std::transform(begin(), end(), v.begin(), begin(), std::plus<F>());
    }

    inline vector operator-(const vector& v) const {
        vector rv;
        std::transform(begin(), end(), v.begin(), rv.begin(), std::minus<F>());
        return rv;
    }

    inline void operator-=(const vector& v) {
        std::transform(begin(), end(), v.begin(), begin(), std::minus<F>());
    }

    inline vector operator-() const {
        vector rv;
        std::transform(begin(), end(), rv.begin(), std::negate<F>());
        return rv;
    }

    inline vector operator*(F c) const {
        vector rv;
        std::transform(begin(), end(), rv.begin(), std::bind2nd(std::multiplies<F>(), c));
        return rv;
    }

    inline vector operator/(F c) const {
        vector rv;
        std::transform(begin(), end(), rv.begin(), std::bind2nd(std::divides<F>(), c));
        return rv;
    }

    inline bool operator==(const vector& v) const {
        return data == v.data;
    }

    inline bool operator!=(const vector& v) const {
        return data != v.data;
    }

    template <typename G> inline operator vector<n, G>() const {
        vector<n, G> rv;
        std::transform(begin(), end(), rv.begin(), [](F v) { return static_cast<G>(v); });
        return rv;
    }
};

template <size_t n, typename F> inline vector<n, F> operator*(F c, const vector<n, F>& vec) {
    return vec * c;
}

template <typename F, typename... G> inline constexpr vector<sizeof...(G) + 1, F> make_vector(F v0, G... v) {
    return vector<sizeof...(G) + 1, F> { v0, static_cast<F>(v)... };
}

template <typename F, typename... G> inline constexpr vector<sizeof...(G), F> make_scaled_vector(F c, G... v) {
    return vector<sizeof...(G), F> { (static_cast<F>(v) * c)... };
}

template <size_t n, typename F = float> inline vector<n, F> make_zero_vector() {
    vector<n, F> rv;
    std::fill(rv.begin(), rv.end(), 0);
    return rv;
}

template <size_t n, typename F = float> inline vector<n, F> make_fill_vector(F c) {
    vector<n, F> rv;
    std::fill(rv.begin(), rv.end(), c);
    return rv;
}

template <size_t n, typename F, size_t m> inline vector<n, F> extend_vector(const vector<m, F>& v, F c) {
    static_assert(m < n, "extended vector must be smaller than resultant vector");
    vector<n, F> rv = make_fill_vector<n, F>(c);
    auto it = v.begin();
    auto jt = rv.begin();
    for(; it != v.end(); ++it, ++jt) {
        *jt = *it;
    }

    return rv;
}

template <unsigned int idx, size_t n, typename F> inline F& get(vector<n, F>& vec) {
    static_assert(idx < n, "index out of bounds");
    return std::get<idx>(vec.data);
}

template <unsigned int idx, size_t n, typename F> inline F get(const vector<n, F>& vec) {
    static_assert(idx < n, "index out of bounds");
    return std::get<idx>(vec.data);
}

template <size_t n, typename F> inline F dot(const vector<n, F>& v, const vector<n, F>& w) {
    F accum = 0;
    for(auto it = v.begin(), jt = w.begin(); it != v.end(); ++it, ++jt) {
        accum += *it * *jt;
    }

    return accum;
}

template <typename F> constexpr vector<3, F> cross(const vector<3, F> & v, const vector<3, F>& w) {
    return make_vector(
            get<1>(v) * get<2>(w) - get<2>(v) * get<1>(w),
            get<2>(v) * get<0>(w) - get<0>(v) * get<2>(w),
            get<0>(v) * get<1>(w) - get<1>(v) * get<0>(w));
}

template <size_t n, typename F> inline constexpr F length_squared(const vector<n, F>& v) {
    return dot(v, v);
}

template <size_t n, typename F> inline constexpr F length(const vector<n, F>& v) {
    return std::sqrt(length_squared(v));
}

template <size_t n, typename F> inline constexpr vector<n, F> normalize(const vector<n, F>& v) {
    return v / length(v);
}

template <size_t m, size_t n, typename F, typename Fn, typename... G> inline typename std::enable_if<(m == n), void>::type
        inner_apply(Fn f, const vector<n, F>&, G... v0) {
    f(v0...);
}

template <size_t m, size_t n, typename F, typename Fn, typename... G> inline typename std::enable_if<(m < n), void>::type
        inner_apply(Fn f, const vector<n, F>& v, G... v0) {
    inner_apply<m + 1>(f, v, v0..., get<m>(v));
}

template <size_t n, typename F, typename Fn> inline void apply(Fn f, const vector<n, F>& v) {
    inner_apply<0>(f, v);
}

template <typename Rv, size_t m, size_t n, typename F, typename Fn, typename... G> inline typename std::enable_if<(m == n), Rv>::type
        inner_apply_v(Fn f, const vector<n, F>&, G... v0) {
    return f(v0...);
}

template <typename Rv, size_t m, size_t n, typename F, typename Fn, typename... G> inline typename std::enable_if<(m < n), Rv>::type
        inner_apply_v(Fn f, const vector<n, F>& v, G... v0) {
    return inner_apply_v<Rv, m + 1>(f, v, v0..., get<m>(v));
}

template <typename Rv, size_t n, typename F, typename Fn> inline Rv apply_v(Fn f, const vector<n, F>& v) {
    return inner_apply_v<Rv, 0>(f, v);
}

template <size_t n, typename F> std::ostream& operator<<(std::ostream& os, const vector<n, F>& vec) {
    os << "(";
    auto it = vec.begin();
    if(it != vec.end()) {
        os << *(it++);
        while(it != vec.end()) {
            os << ", " << *(it++);
        }
    }
    os << ")";
    return os;
}

}
}

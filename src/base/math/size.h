#pragma once

#include <array>
#include <initializer_list>
#include <utility>
#include <algorithm>

namespace gorc {
inline namespace math {

template <size_t n, typename F = float> class size {
    template <size_t m, typename G> friend size<m, G> operator*(G, const size<m, G>&);
    template <unsigned int idx, size_t m, typename G> friend G& get(size<m, G>&);
    template <unsigned int idx, size_t m, typename G> friend G get(const size<m, G>&);
    template <size_t m, typename G> friend size<m, G> make_zero_size();
    template <size_t m, typename G> friend size<m, G> make_fill_size(G);

private:
    std::array<F, n> data;

public:
    constexpr size() { }
    size(const size&) = default;

    template <typename... G, typename = typename std::enable_if<sizeof...(G) == n>::type>
    constexpr explicit size(G&&... v)
        : data {{ std::forward<G>(v)... }} { }

    size& operator=(const size&) = default;

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

    inline size operator*(F c) const {
        size rv;
        std::transform(begin(), end(), rv.begin(), std::bind2nd(std::multiplies<F>(), c));
        return rv;
    }

    inline void operator*=(F c) {
        std::transform(begin(), end(), begin(), std::bind2nd(std::multiplies<F>(), c));
    }

    inline size operator/(F c) const {
        size rv;
        std::transform(begin(), end(), rv.begin(), std::bind2nd(std::divides<F>(), c));
        return rv;
    }

    inline void operator/=(F c) {
        std::transform(begin(), end(), begin(), std::bind2nd(std::divides<F>(), c));
    }

    inline bool operator==(const size& v) const {
        return data == v.data;
    }

    inline bool operator!=(const size& v) const {
        return data != v.data;
    }

    template <typename G> inline operator size<n, G>() const {
        size<n, G> rv;
        std::transform(begin(), end(), rv.begin(), [](F v) { return static_cast<G>(v); });
        return rv;
    }
};

template <size_t n, typename F> inline size<n, F> operator*(F c, const size<n, F>& vec) {
    return vec * c;
}

template <typename F, typename... G> inline constexpr size<sizeof...(G) + 1, F> make_size(F v0, G... v) {
    return size<sizeof...(G) + 1, F> { v0, static_cast<F>(v)... };
}

template <unsigned int idx, size_t n, typename F> inline F& get(size<n, F>& vec) {
    static_assert(idx < n, "index out of bounds");
    return std::get<idx>(vec.data);
}

template <unsigned int idx, size_t n, typename F> inline F get(const size<n, F>& vec) {
    static_assert(idx < n, "index out of bounds");
    return std::get<idx>(vec.data);
}

}
}

#pragma once

#include "vector.h"
#include <array>
#include <tuple>

namespace gorc {
inline namespace math {

template <size_t n, typename F = float> class box {
    template <size_t m, size_t o, typename G> friend std::tuple<G, G> get_range(const box<o, G>&);
    template <size_t m, size_t o, typename G> friend G get_size(const box<o, G>&);

public:
    vector<n, F> v0, v1;

    constexpr box() { }
    constexpr box(const vector<n, F>& v0, const vector<n, F>& v1) : v0(v0), v1(v1) { }

    bool contains(const vector<n, F>& v) {
        auto it = v0.begin();
        auto jt = v1.begin();
        auto rt = v.begin();

        for(; it != v0.end(); ++it, ++jt, ++rt) {
            if(*rt < *it || *rt > *jt) {
                return false;
            }
        }

        return true;
    }

    box operator+(const vector<n, F>& v) const {
        return box(v0 + v, v1 + v);
    }

    box operator-(const vector<n, F>& v) const {
        return box(v0 - v, v1 - v);
    }

    bool operator==(const box& b) const {
        return v0 == b.v0 && v1 == b.v1;
    }

    bool operator!=(const box& b) const {
        return v0 != b.v0 || v1 != b.v1;
    }

    box operator&(const box& b) const {
        vector<n, F> nv0, nv1;

        auto v0_it = v0.begin(), v1_it = v1.begin(), b0_it = b.v0.begin(), b1_it = b.v1.begin();
        auto nv0_it = nv0.begin(), nv1_it = nv1.begin();
        for(; v0_it != v0.end(); ++v0_it, ++v1_it, ++b0_it, ++b1_it, ++nv0_it, ++nv1_it) {
            *nv0_it = std::max(*v0_it, *b0_it);
            *nv1_it = std::min(*v1_it, *b1_it);
        }

        return box(nv0, nv1);
    }

    bool overlaps(const box& b) const {
        auto v0_it = v0.begin(), v1_it = v1.begin(), b0_it = b.v0.begin(), b1_it = b.v1.begin();
        for(; v0_it != v0.end(); ++v0_it, ++v1_it, ++b0_it, ++b1_it) {
            if(*v1_it < *b0_it || *v0_it > *b1_it) {
                return false;
            }
        }

        return true;
    }
};

template <size_t n, typename T> inline constexpr box<n, T> make_box(const vector<n, T>& v0, const vector<n, T>& v1) {
    return box<n, T>(v0, v1);
}

template <size_t m, size_t n, typename T> std::tuple<T, T> get_range(const box<n, T>& box) {
    static_assert(m < n, "box dimension out of bounds");
    return std::make_tuple(get<m>(box.v0), get<m>(box.v1));
}

template <size_t m, size_t n, typename T> T get_size(const box<n, T>& box) {
    static_assert(m < n, "box dimension out of bounds");
    return get<m>(box.v1) - get<m>(box.v0);
}

}
}

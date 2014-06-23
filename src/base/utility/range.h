#pragma once

namespace gorc {
inline namespace utility {

template <typename It> class range {
private:
    It first, last;

public:
    range(const It& first, const It& last)
        : first(first), last(last) {
        return;
    }

    const It& begin() const {
        return first;
    }

    const It& end() const {
        return last;
    }

    inline bool empty() const {
        return first == last;
    }
};

template <typename It> range<It> make_range(const It& first, const It& last) {
    return range<It>(first, last);
}

template <typename TupleT> auto make_range(const TupleT& tuple) -> decltype(make_range(std::get<0>(tuple), std::get<1>(tuple))) {
    return make_range(std::get<0>(tuple), std::get<1>(tuple));
}

template <typename RangeT> auto make_range(RangeT& rng) -> decltype(make_range(rng.begin(), rng.end())) {
    return make_range(rng.begin(), rng.end());
}

}
}

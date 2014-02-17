#pragma once

#include <utility>

namespace gorc {
inline namespace utility {

template <typename T> class pair_range {
private:
    std::pair<T, T> m_pair;

public:
    explicit pair_range(const std::pair<T, T>& m_pair) : m_pair(m_pair) {
        return;
    }

    T begin() const {
        return std::get<0>(m_pair);
    }

    T end() const {
        return std::get<1>(m_pair);
    }

    bool empty() const {
        return std::get<0>(m_pair) == std::get<1>(m_pair);
    }
};

template <typename T> class tuple_range {
private:
    std::tuple<T, T> m_pair;

public:
    explicit tuple_range(const std::tuple<T, T>& m_pair) : m_pair(m_pair) {
        return;
    }

    T begin() const {
        return std::get<0>(m_pair);
    }

    T end() const {
        return std::get<1>(m_pair);
    }

    bool empty() const {
        return std::get<0>(m_pair) == std::get<1>(m_pair);
    }
};

template <typename T> pair_range<T> make_pair_range(const std::pair<T, T>& p) {
    return pair_range<T>(p);
}

template <typename T> tuple_range<T> make_pair_range(const std::tuple<T, T>& p) {
    return tuple_range<T>(p);
}

template <typename T> pair_range<T> make_pair_range(const T& begin, const T& end) {
    return pair_range<T>(std::make_pair(begin, end));
}

}
}

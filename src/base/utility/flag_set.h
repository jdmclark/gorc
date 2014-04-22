#pragma once

#include <initializer_list>
#include <type_traits>

namespace gorc {
inline namespace utility {

template <typename T> class flag_set {
    using UT = typename std::underlying_type<T>::type;
private:
    UT value;

public:
    flag_set() : value(0) {
        return;
    }

    explicit flag_set(UT forced_value) : value(forced_value) {
        return;
    }

    flag_set(std::initializer_list<T> initl) {
        value = 0;
        for(T val : initl) {
            value |= static_cast<UT>(val);
        }
    }

    explicit operator UT() const {
        return value;
    }

    inline bool operator&(T val) const {
        return value & static_cast<UT>(val);
    }

    inline bool operator&(flag_set val) const {
        return value & val.value;
    }

    inline flag_set operator+(T val) const {
        return flag_set(value | static_cast<UT>(val));
    }

    inline const flag_set& operator+=(T val) {
        value |= static_cast<UT>(val);
        return *this;
    }

    inline flag_set operator+(flag_set val) const {
        return flag_set(value | val.value);
    }

    inline const flag_set& operator+=(flag_set val) {
        value |= val.value;
        return *this;
    }

    inline flag_set operator-(T val) const {
        return flag_set(value & ~static_cast<UT>(val));
    }

    inline const flag_set& operator-=(T val) {
        value &= ~static_cast<UT>(val);
        return *this;
    }

    inline flag_set operator-(flag_set val) const {
        return flag_set(value & ~val.value);
    }

    inline const flag_set& operator-=(flag_set val) {
        value &= ~val.value;
        return *this;
    }
};

}
}

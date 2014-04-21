#pragma once

#include <type_traits>
#include <utility>

namespace gorc {
inline namespace utility {

struct nothing_type {
};

constexpr nothing_type nothing = nothing_type();

template <typename T, typename Enable = void> class maybe;

template <typename T> class maybe<T, typename std::enable_if<std::is_pointer<T>::value>::type> {
private:
    T value;

public:
    maybe() : value(nullptr) {
        return;
    }

    maybe(nothing_type) : value(nullptr) {
        return;
    }

    maybe(const T& value) : value(value) {
        return;
    }

    T operator=(T value) {
        this->value = value;
        return value;
    }

    maybe<T>& operator=(const nothing_type&) {
        this->value = nullptr;
        return *this;
    }

    inline bool operator==(const maybe<T>& o) const {
        return value == o.value;
    }

    inline bool operator==(const T& v) const {
        return value == v;
    }

    inline bool has_value() const {
        return value != nullptr;
    }

    inline operator const T() const {
        return value;
    }

    inline operator T() {
        return value;
    }
};

template <typename T> class maybe<T, typename std::enable_if<!std::is_pointer<T>::value>::type> {
private:
    T value;
    bool has_value_ = false;

public:
    maybe() : has_value_(false) {
        return;
    }

    maybe(nothing_type) : has_value_(false) {
        return;
    }

    maybe(const T& value) : value(value), has_value_(true) {
        return;
    }

    const T& operator=(const T& value) {
        this->value = value;
        has_value_ = true;
        return value;
    }

    maybe<T>& operator=(const nothing_type&) {
        this->has_value_ = false;
        return *this;
    }

    inline bool operator==(const maybe<T>& o) const {
        if(!has_value_ && !o.has_value_) {
            return true;
        }
        else if(has_value_ && o.has_value_) {
            return value == o.value;
        }
        else {
            return false;
        }
    }

    inline bool operator==(const T& v) const {
        if(has_value_) {
            return value == v;
        }
        else {
            return false;
        }
    }

    inline bool has_value() const {
        return has_value_;
    }

    inline operator const T*() const {
        if(has_value_) {
            return &value;
        }
        else {
            return nullptr;
        }
    }

    inline operator T*() {
        if(has_value_) {
            return &value;
        }
        else {
            return nullptr;
        }
    }
};

template <typename T> maybe<T> make_maybe(const T& value) {
    return maybe<T>(value);
}

}
}

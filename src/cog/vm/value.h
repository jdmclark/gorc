#pragma once

#include "base/math/vector.h"
#include "type.h"
#include <functional>

namespace gorc {
namespace cog {
namespace vm {

class value {
private:
    type type_flag;

    union {
        int integer;
        float floating;
        bool boolean;
        const char* string;
        struct {
            float x, y, z;
        } vector;
    } data;

public:
    value();

    value(const value&);
    const value& operator=(const value&);

    value(int);
    int operator=(int);
    operator int() const;

    value(float);
    float operator=(float);
    operator float() const;

    value(bool);
    bool operator=(bool);
    operator bool() const;

    value(const char*);
    const char* operator=(const char*);
    operator const char*() const;

    value(const vector<3>&);
    const vector<3>& operator=(const vector<3>&);
    operator vector<3>() const;

    inline bool is_numeric() const {
        return type_flag == type::integer || type_flag == type::floating;
    }

    inline type get_type() const {
        return type_flag;
    }

    inline value operator+(const value& v2) const {
        if(type_flag == type::floating || v2.type_flag == type::floating) {
            return value(static_cast<float>(*this) + static_cast<float>(v2));
        }
        else {
            return value(static_cast<int>(*this) + static_cast<int>(v2));
        }
    }

    inline value operator-(const value& v2) const {
        if(type_flag == type::floating || v2.type_flag == type::floating) {
            return value(static_cast<float>(*this) - static_cast<float>(v2));
        }
        else {
            return value(static_cast<int>(*this) - static_cast<int>(v2));
        }
    }

    inline value operator*(const value& v2) const {
        if(type_flag == type::floating || v2.type_flag == type::floating) {
            return value(static_cast<float>(*this) * static_cast<float>(v2));
        }
        else {
            return value(static_cast<int>(*this) * static_cast<int>(v2));
        }
    }

    inline value operator/(const value& v2) const {
        if(type_flag == type::floating || v2.type_flag == type::floating) {
            return value(static_cast<float>(*this) / static_cast<float>(v2));
        }
        else {
            return value(static_cast<int>(*this) / static_cast<int>(v2));
        }
    }

    inline value operator%(const value& v2) const {
        return value(static_cast<int>(*this) % static_cast<int>(v2));
    }

    inline value operator&(const value& v2) const {
        return value(static_cast<int>(*this) & static_cast<int>(v2));
    }

    inline value operator|(const value& v2) const {
        return value(static_cast<int>(*this) | static_cast<int>(v2));
    }

    inline value operator^(const value& v2) const {
        return value(static_cast<int>(*this) ^ static_cast<int>(v2));
    }

    inline value operator>(const value& v2) const {
        if(type_flag == type::floating || v2.type_flag == type::floating) {
            return value(static_cast<float>(*this) > static_cast<float>(v2));
        }
        else {
            return value(static_cast<int>(*this) > static_cast<int>(v2));
        }
    }

    inline value operator>=(const value& v2) const {
        if(type_flag == type::floating || v2.type_flag == type::floating) {
            return value(static_cast<float>(*this) >= static_cast<float>(v2));
        }
        else {
            return value(static_cast<int>(*this) >= static_cast<int>(v2));
        }
    }

    inline value operator<(const value& v2) const {
        if(type_flag == type::floating || v2.type_flag == type::floating) {
            return value(static_cast<float>(*this) < static_cast<float>(v2));
        }
        else {
            return value(static_cast<int>(*this) < static_cast<int>(v2));
        }
    }

    inline value operator<=(const value& v2) const {
        if(type_flag == type::floating || v2.type_flag == type::floating) {
            return value(static_cast<float>(*this) <= static_cast<float>(v2));
        }
        else {
            return value(static_cast<int>(*this) <= static_cast<int>(v2));
        }
    }

    inline value operator==(const value& v2) const {
        if(type_flag == type::floating || v2.type_flag == type::floating) {
            return value(std::equal_to<float>()(static_cast<float>(*this), static_cast<float>(v2)));
        }
        else if(type_flag == type::integer || v2.type_flag == type::integer) {
            return value(static_cast<int>(*this) == static_cast<int>(v2));
        }
        else if(type_flag == type::boolean || v2.type_flag == type::boolean) {
            return value(static_cast<bool>(*this) == static_cast<bool>(v2));
        }
        else {
            return value(false);
        }
    }

    inline value operator!=(const value& v2) const {
        if(type_flag == type::floating || v2.type_flag == type::floating) {
            return value(std::not_equal_to<float>()(static_cast<float>(*this), static_cast<float>(v2)));
        }
        else if(type_flag == type::integer || v2.type_flag == type::integer) {
            return value(static_cast<int>(*this) != static_cast<int>(v2));
        }
        else if(type_flag == type::boolean || v2.type_flag == type::boolean) {
            return value(static_cast<bool>(*this) != static_cast<bool>(v2));
        }
        else {
            return value(false);
        }
    }

    inline value operator&&(const value& v2) const {
        return value(static_cast<bool>(*this) && static_cast<bool>(v2));
    }

    inline value operator||(const value& v2) const {
        return value(static_cast<bool>(*this) || static_cast<bool>(v2));
    }

    inline value operator+() const {
        return *this;
    }

    inline value operator-() const {
        if(type_flag == type::floating) {
            return value(-static_cast<float>(*this));
        }
        else {
            return value(-static_cast<int>(*this));
        }
    }

    inline value operator!() const {
        return value(!static_cast<bool>(*this));
    }
};

}
}
}

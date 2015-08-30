#pragma once

#include "type.hpp"
#include "id.hpp"
#include "math/vector.hpp"
#include <string>

namespace gorc {
    namespace cog {

        class value {
        private:
            value_type type_flag = value_type::nothing;

            union {
                int integer;
                float floating;
                bool boolean;
                char const *string;
                struct {
                    float x, y, z;
                } vector;
            } data;

        public:
            value() = default;
            value(value const &) = default;
            value& operator=(value const &) = default;

            value(char const *);
            value& operator=(char const *);
            operator char const*() const;

            value(bool);
            value& operator=(bool);
            operator bool() const;

            value(int);
            value& operator=(int);
            operator int() const;

            value(float);
            value& operator=(float);
            operator float() const;

            value(vector<3> const &);
            value& operator=(vector<3> const &);
            operator vector<3>() const;

#define MAKE_ID_MEMBERS(x) \
            value(x##_id); \
            value& operator=(x##_id); \
            operator x##_id() const

            MAKE_ID_MEMBERS(sector);
            MAKE_ID_MEMBERS(surface);
            MAKE_ID_MEMBERS(thing);
            MAKE_ID_MEMBERS(player);
            MAKE_ID_MEMBERS(ai);
            MAKE_ID_MEMBERS(cog);
            MAKE_ID_MEMBERS(keyframe);
            MAKE_ID_MEMBERS(material);
            MAKE_ID_MEMBERS(model);
            MAKE_ID_MEMBERS(sound);
            MAKE_ID_MEMBERS(thing_template);
            MAKE_ID_MEMBERS(message);

#undef MAKE_ID_MEMBERS

            value operator+(value const &) const;
            value operator-(value const &) const;
            value operator*(value const &) const;
            value operator/(value const &) const;
            value operator%(value const &) const;
            value operator&(value const &) const;
            value operator|(value const &) const;
            value operator^(value const &) const;
            value operator>(value const &) const;
            value operator<(value const &) const;
            value operator>=(value const &) const;
            value operator<=(value const &) const;
            value operator==(value const &) const;
            value operator!=(value const &) const;
            value operator&&(value const &) const;
            value operator||(value const &) const;
            value operator+() const;
            value operator-() const;
            value operator!() const;

            value_type get_type() const;
            std::string as_string() const;

            bool is_same(value const &v) const;
        };

        std::string as_string(value const &);
        bool is_same(value const &u, value const &v);
    }
}
#pragma once

#include <initializer_list>
#include <type_traits>

namespace gorc {

    template <typename T>
    class flag_set {
        using UT = typename std::underlying_type<T>::type;
    private:
        UT value;

    public:
        flag_set() : value(0) { }

        explicit flag_set(UT forced_value)
            : value(forced_value) { }

        flag_set(std::initializer_list<T> initl)
        {
            value = 0;
            for(T val : initl) {
                value |= static_cast<UT>(val);
            }
        }

        explicit operator T() const
        {
            return T(value);
        }

        explicit operator UT() const
        {
            return value;
        }

        inline bool operator&(T val) const
        {
            return value & static_cast<UT>(val);
        }

        inline bool operator&(flag_set val) const
        {
            return value & val.value;
        }

        inline flag_set operator+(T val) const
        {
            return flag_set(value | static_cast<UT>(val));
        }

        inline flag_set operator+(flag_set val) const
        {
            return flag_set(value | val.value);
        }

        inline flag_set const &operator+=(T val)
        {
            value |= static_cast<UT>(val);
            return *this;
        }

        inline flag_set const &operator+=(flag_set val)
        {
            value |= val.value;
            return *this;
        }

        inline flag_set operator-(T val) const
        {
            return flag_set(value & ~static_cast<UT>(val));
        }

        inline flag_set operator-(flag_set val) const
        {
            return flag_set(value & ~val.value);
        }

        inline flag_set const &operator-=(T val)
        {
            value = value & ~static_cast<UT>(val);
            return *this;
        }

        inline flag_set const &operator-=(flag_set val)
        {
            value = value & ~val.value;
            return *this;
        }

        inline bool operator==(flag_set val) const
        {
            return value == val.value;
        }

        inline bool operator!=(flag_set val) const
        {
            return value != val.value;
        }
    };

    namespace detail {
        template <typename U>
        U fake_flag_set_enum_type(flag_set<U> const &);
    }

    template <typename T>
    struct flag_set_enum_type {
        using type = decltype(detail::fake_flag_set_enum_type(std::declval<T>()));
    };

    template <typename T>
    struct flag_set_underlying_type {
        using type = typename std::underlying_type<typename flag_set_enum_type<T>::type>::type;
    };

    template <typename T>
    struct is_flag_set {
    private:
        using yes = char[1];
        using no = char[2];

        template <typename U,
                  typename = decltype(detail::fake_flag_set_enum_type(std::declval<U>()))>
        static yes& test(int);

        template <typename U>
        static no& test(...);

    public:
        static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes);
    };

}

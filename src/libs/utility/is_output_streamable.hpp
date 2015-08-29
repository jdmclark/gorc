#pragma once

#include <type_traits>
#include <iostream>

namespace gorc {
    template <typename T>
    struct is_output_streamable {
    private:
        using yes = char[1];
        using no = char[2];

        template <typename U>
        static U const& get_value();

        template <typename U>
        static std::ostream& get_ostream();

        template <typename U, typename = decltype(get_ostream<U>() << get_value<U>())>
        static yes& test(int);

        template <typename U>
        static no& test(...);

    public:
        static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes);
    };
}

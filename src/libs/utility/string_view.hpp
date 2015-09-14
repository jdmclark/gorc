#pragma once

#include <cstddef>

namespace gorc {

    class string_view {
    private:
        char const *first;
        char const *last;

    public:
        inline constexpr string_view(char const *str, size_t len)
            : first(str)
            , last(str + len)
        {
        }

        char const* begin() const;
        char const* end() const;
    };

    inline constexpr string_view operator"" _sv(char const *str, size_t len)
    {
        return string_view(str, len);
    }
}

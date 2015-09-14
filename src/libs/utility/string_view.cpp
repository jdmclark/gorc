#include "string_view.hpp"

char const* gorc::string_view::begin() const
{
    return first;
}

char const* gorc::string_view::end() const
{
    return last;
}

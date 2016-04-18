#include "loader.hpp"

gorc::loader::~loader()
{
    return;
}

gorc::maybe<char const *> gorc::loader::get_default() const
{
    return nothing;
}

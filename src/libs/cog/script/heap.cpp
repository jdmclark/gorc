#include "heap.hpp"

gorc::cog::heap::heap(size_t initial_size)
    : values(initial_size)
{
    return;
}

gorc::cog::value& gorc::cog::heap::operator[](size_t index)
{
    if(index >= values.size()) {
        values.resize(index + 1);
    }

    return values[index];
}

gorc::cog::value gorc::cog::heap::operator[](size_t index) const
{
    if(index >= values.size()) {
        return value();
    }

    return values[index];
}

size_t gorc::cog::heap::size() const
{
    return values.size();
}

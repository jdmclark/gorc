#include "output_stream.hpp"
#include "utility/runtime_assert.hpp"
#include <cstdio>
#include <fcntl.h>

gorc::output_stream::~output_stream()
{
    return;
}

void gorc::output_stream::write(void const *src, size_t size)
{
    runtime_assert(write_some(src, size) == size,
                   "output_stream::write system error");
}

#include "std_output_stream.hpp"
#include <cstdio>
#include <fcntl.h>

gorc::std_output_stream::~std_output_stream()
{
    std::fflush(stdout);
}

size_t gorc::std_output_stream::write_some(void const *src, size_t size)
{
    return std::fwrite(src, 1U, size, stdout);
}

void gorc::std_output_stream::reopen_as_binary()
{
#ifdef PLATFORM_MINGW
    _setmode(_fileno(stdout), _O_BINARY);
#endif
}

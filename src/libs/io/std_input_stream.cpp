#include "std_input_stream.hpp"
#include <cstdio>
#include <fcntl.h>

size_t gorc::std_input_stream::read_some(void *dest, size_t size)
{
    return std::fread(dest, 1U, size, stdin);
}

bool gorc::std_input_stream::at_end()
{
    return std::feof(stdin);
}

void gorc::std_input_stream::reopen_as_binary()
{
#ifdef PLATFORM_MINGW
    _setmode(_fileno(stdin), _O_BINARY);
#endif
}

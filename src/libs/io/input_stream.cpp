#include "input_stream.hpp"
#include "output_stream.hpp"
#include <stdexcept>
#include <cstdio>
#include <fcntl.h>

gorc::input_stream::~input_stream()
{
    return;
}

void gorc::input_stream::read(void *dest, size_t size)
{
    if(read_some(dest, size) != size) {
        char const *msg = at_end() ?
                          "input_stream::read size exceeds bounds" :
                          "input_stream::read system error";
        throw std::runtime_error(msg);
    }
}

void gorc::input_stream::copy_to(output_stream &os)
{
    char buffer[1024];
    while(!at_end()) {
        size_t amt = read_some(buffer, 1024U);
        os.write(buffer, amt);
    }
}

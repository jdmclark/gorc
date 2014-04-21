#include "output_stream.h"

gorc::io::output_stream::~output_stream() {
    return;
}

gorc::io::std_output_stream::~std_output_stream() {
    std::fflush(stdout);
}

void gorc::io::std_output_stream::write(const void* src, size_t size) {
    std::fwrite(src, size, 1, stdout);
}

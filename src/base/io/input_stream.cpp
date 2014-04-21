#include "input_stream.h"
#include "exception.h"

gorc::io::input_stream::~input_stream() {
    return;
}

void gorc::io::std_input_stream::read(void* dest, size_t size) {
    if(std::fread(dest, size, 1, stdin) != 1) {
        throw file_read_exception();
    }
}

bool gorc::io::std_input_stream::at_end() {
    return std::feof(stdin);
}

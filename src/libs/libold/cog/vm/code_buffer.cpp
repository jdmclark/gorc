#include "code_buffer.hpp"
#include "exception.hpp"
#include <cstring>

gorc::cog::vm::code_buffer::code_buffer()
    : data(128, 0), topPtr(0) {
    return;
}

void gorc::cog::vm::code_buffer::throwCodeBufferOverflowException() const {
    throw code_buffer_overflow_exception();
}

void gorc::cog::vm::code_buffer::write(size_t dest, const void* src, size_t size) {
    resizeDataForWrite(dest, size);
    memcpy(&data[dest], src, size);
}

void gorc::cog::vm::code_buffer::read(void* dest, size_t src, size_t size) const {
    tryRead(src, size);
    memcpy(dest, &data[src], size);
}

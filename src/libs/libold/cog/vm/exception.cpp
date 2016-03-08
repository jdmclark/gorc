#include "exception.hpp"

const char* gorc::cog::vm::code_buffer_overflow_exception::what() const throw() {
    return "code buffer overflow";
}

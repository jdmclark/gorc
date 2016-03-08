#include "code_buffer_read_stream.hpp"

gorc::cog::vm::code_buffer_read_stream::code_buffer_read_stream(const code_buffer& cBuffer)
    : codeBuffer(cBuffer), streamPtr(0)
{
    return;
}

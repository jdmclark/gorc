#include "code_buffer_write_stream.h"

gorc::cog::vm::code_buffer_write_stream::code_buffer_write_stream(code_buffer& cBuffer)
    : codeBuffer(cBuffer), streamPtr(0)
{
    return;
}

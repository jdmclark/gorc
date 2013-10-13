#include "code_buffer_read_stream.h"

Gorc::Cog::VM::CodeBufferReadStream::CodeBufferReadStream(const CodeBuffer& cBuffer)
	: codeBuffer(cBuffer), streamPtr(0)
{
	return;
}

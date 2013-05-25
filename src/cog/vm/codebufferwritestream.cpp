#include "codebufferwritestream.h"

Gorc::Cog::VM::CodeBufferWriteStream::CodeBufferWriteStream(CodeBuffer& cBuffer)
	: codeBuffer(cBuffer), streamPtr(0)
{
	return;
}

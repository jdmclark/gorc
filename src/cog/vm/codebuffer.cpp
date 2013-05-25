#include "codebuffer.h"
#include "exception.h"
#include <cstring>

Gorc::Cog::VM::CodeBuffer::CodeBuffer()
	: data(128, 0), topPtr(0) {
	return;
}

void Gorc::Cog::VM::CodeBuffer::throwCodeBufferOverflowException() const {
	throw CodeBufferOverflowException();
}

void Gorc::Cog::VM::CodeBuffer::Write(size_t dest, const void* src, size_t size) {
	resizeDataForWrite(dest, size);
	memcpy(&data[dest], src, size);
}

void Gorc::Cog::VM::CodeBuffer::Read(void* dest, size_t src, size_t size) const {
	tryRead(src, size);
	memcpy(dest, &data[src], size);
}

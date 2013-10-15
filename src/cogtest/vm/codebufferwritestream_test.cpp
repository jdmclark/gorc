#include <nullunit/nullunit.h>
#include "cog/vm/code_buffer_write_stream.h"
#include "cog/vm/code_buffer_read_stream.h"

BeginSuite(CodeBufferWriteStreamTests);

Case(seek) {
	gorc::cog::vm::code_buffer cBuffer;
	gorc::cog::vm::code_buffer_write_stream cb(cBuffer);

	cb.seek(128);
	Test_Assert_Eq(cb.tell(), 128);
}

Case(WriteStream) {
	gorc::cog::vm::code_buffer cBuffer;
	gorc::cog::vm::code_buffer_write_stream cb(cBuffer);

	std::string hello = "Hello, world!\n";

	cb.write(hello.c_str(), static_cast<size_t>(hello.length()));
	cb.write(hello.c_str(), static_cast<size_t>(hello.length()));
}

Case(WriteStreamTemplated) {
	gorc::cog::vm::code_buffer cBuffer;
	gorc::cog::vm::code_buffer_write_stream cb(cBuffer);

	cb.write<uint32_t>(0xFEEDBEEF);
	cb.write<uint32_t>(0xABACADAB);
	cb.write<uint32_t>(0xF0F0F0F0);
}

Case(write_buffer) {
	gorc::cog::vm::code_buffer cBufferOne;
	cBufferOne.write<uint32_t>(0xFEEDBEEF, 0);

	gorc::cog::vm::code_buffer cBuffer;
	gorc::cog::vm::code_buffer_write_stream cb(cBuffer);
	size_t destOffset = cb.write_buffer(cBufferOne);
	size_t destOffset2 = cb.write_buffer(cBufferOne);

	Test_Assert_Eq(cBuffer.read<uint32_t>(0), 0xFEEDBEEF);
	Test_Assert_Eq(cBuffer.read<uint32_t>(sizeof(uint32_t)), 0xFEEDBEEF);
	Test_Assert_Eq(destOffset, 0);
	Test_Assert_Eq(destOffset2, sizeof(uint32_t));
}

EndSuite(CodeBufferWriteStreamTests);

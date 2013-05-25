#include <nullunit/nullunit.h>
#include "cog/vm/codebufferwritestream.h"
#include "cog/vm/codebufferreadstream.h"

BeginSuite(CodeBufferWriteStreamTests);

Case(Seek) {
	Gorc::Cog::VM::CodeBuffer cBuffer;
	Gorc::Cog::VM::CodeBufferWriteStream cb(cBuffer);

	cb.Seek(128);
	Test_Assert_Eq(cb.Tell(), 128);
}

Case(WriteStream) {
	Gorc::Cog::VM::CodeBuffer cBuffer;
	Gorc::Cog::VM::CodeBufferWriteStream cb(cBuffer);

	std::string hello = "Hello, World!\n";

	cb.Write(hello.c_str(), static_cast<size_t>(hello.length()));
	cb.Write(hello.c_str(), static_cast<size_t>(hello.length()));
}

Case(WriteStreamTemplated) {
	Gorc::Cog::VM::CodeBuffer cBuffer;
	Gorc::Cog::VM::CodeBufferWriteStream cb(cBuffer);

	cb.Write<uint32_t>(0xFEEDBEEF);
	cb.Write<uint32_t>(0xABACADAB);
	cb.Write<uint32_t>(0xF0F0F0F0);
}

Case(WriteBuffer) {
	Gorc::Cog::VM::CodeBuffer cBufferOne;
	cBufferOne.Write<uint32_t>(0xFEEDBEEF, 0);

	Gorc::Cog::VM::CodeBuffer cBuffer;
	Gorc::Cog::VM::CodeBufferWriteStream cb(cBuffer);
	size_t destOffset = cb.WriteBuffer(cBufferOne);
	size_t destOffset2 = cb.WriteBuffer(cBufferOne);

	Test_Assert_Eq(cBuffer.Read<uint32_t>(0), 0xFEEDBEEF);
	Test_Assert_Eq(cBuffer.Read<uint32_t>(sizeof(uint32_t)), 0xFEEDBEEF);
	Test_Assert_Eq(destOffset, 0);
	Test_Assert_Eq(destOffset2, sizeof(uint32_t));
}

EndSuite(CodeBufferWriteStreamTests);

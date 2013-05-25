#include <nullunit/nullunit.h>
#include "cog/vm/codebufferreadstream.h"
#include "cog/vm/codebufferwritestream.h"

BeginSuite(CodeBufferReadStreamTests);

Case(Seek) {
	Gorc::Cog::VM::CodeBuffer cBuffer;
	Gorc::Cog::VM::CodeBufferReadStream cb(cBuffer);

	cb.Seek(128);
	Test_Assert_Eq(cb.Tell(), 128);
}

Case(ReadStream) {
	Gorc::Cog::VM::CodeBuffer cBuffer;
	Gorc::Cog::VM::CodeBufferWriteStream cb(cBuffer);
	Gorc::Cog::VM::CodeBufferReadStream db(cBuffer);

	uint32_t value = 0xFEEDBEEF;

	cb.Write(&value, sizeof(uint32_t));

	uint32_t readValue = 0;

	db.Read(&readValue, sizeof(uint32_t));

	Test_Assert_Eq(readValue, 0xFEEDBEEF);
}

Case(ReadStreamTemplated) {
	Gorc::Cog::VM::CodeBuffer cBuffer;
	Gorc::Cog::VM::CodeBufferWriteStream cb(cBuffer);
	Gorc::Cog::VM::CodeBufferReadStream db(cBuffer);

	cb.Write<uint32_t>(0xFEEDBEEF);
	cb.Write<uint32_t>(0xABACADAB);
	cb.Write<uint32_t>(0xF0F0F0F0);

	Test_Assert_Eq(db.Read<uint32_t>(), 0xFEEDBEEF);
	Test_Assert_Eq(db.Read<uint32_t>(), 0xABACADAB);
	Test_Assert_Eq(db.Read<uint32_t>(), 0xF0F0F0F0);
}

EndSuite(CodeBufferReadStreamTests);

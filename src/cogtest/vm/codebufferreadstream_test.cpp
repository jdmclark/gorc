#include <nullunit/nullunit.h>
#include "cog/vm/code_buffer_read_stream.h"
#include "cog/vm/code_buffer_write_stream.h"

BeginSuite(CodeBufferReadStreamTests);

Case(seek) {
	gorc::cog::vm::code_buffer cBuffer;
	gorc::cog::vm::code_buffer_read_stream cb(cBuffer);

	cb.seek(128);
	Test_Assert_Eq(cb.tell(), 128);
}

Case(ReadStream) {
	gorc::cog::vm::code_buffer cBuffer;
	gorc::cog::vm::code_buffer_write_stream cb(cBuffer);
	gorc::cog::vm::code_buffer_read_stream db(cBuffer);

	uint32_t value = 0xFEEDBEEF;

	cb.write(&value, sizeof(uint32_t));

	uint32_t readvalue = 0;

	db.read(&readvalue, sizeof(uint32_t));

	Test_Assert_Eq(readvalue, 0xFEEDBEEF);
}

Case(ReadStreamTemplated) {
	gorc::cog::vm::code_buffer cBuffer;
	gorc::cog::vm::code_buffer_write_stream cb(cBuffer);
	gorc::cog::vm::code_buffer_read_stream db(cBuffer);

	cb.write<uint32_t>(0xFEEDBEEF);
	cb.write<uint32_t>(0xABACADAB);
	cb.write<uint32_t>(0xF0F0F0F0);

	Test_Assert_Eq(db.read<uint32_t>(), 0xFEEDBEEF);
	Test_Assert_Eq(db.read<uint32_t>(), 0xABACADAB);
	Test_Assert_Eq(db.read<uint32_t>(), 0xF0F0F0F0);
}

EndSuite(CodeBufferReadStreamTests);

#include <nullunit/nullunit.h>
#include "cog/vm/code_buffer.h"
#include "cog/vm/exception.h"

#include <string>
#include <cstring>

BeginSuite(CodeBufferTests);

Case(Write) {
	Gorc::Cog::VM::CodeBuffer cb;

	std::string hello = "Hello, World!\n";

	cb.Write(0, hello.c_str(), static_cast<size_t>(hello.length()));
	cb.Write(96, hello.c_str(), static_cast<size_t>(hello.length()));
}

Case(WriteTemplated) {
	Gorc::Cog::VM::CodeBuffer cb;
	cb.Write<uint32_t>(0xFEEDBEEF, 5);
}

Case(Read) {
	Gorc::Cog::VM::CodeBuffer cb;

	uint32_t value = 0xFEEDBEEF;

	cb.Write(0, &value, sizeof(uint32_t));

	uint32_t readValue = 0;

	cb.Read(&readValue, 0, sizeof(uint32_t));

	Test_Assert_Eq(readValue, 0xFEEDBEEF);
}

Case(ReadFailure) {
	Gorc::Cog::VM::CodeBuffer cb;

	uint32_t value = 0xFEEDBEEF;

	cb.Write(0, &value, sizeof(uint32_t));

	try {
		uint32_t readValue = 0;
		cb.Read(&readValue, 2, sizeof(uint32_t));
	}
	catch(Gorc::Cog::VM::CodeBufferOverflowException&) {
		return;
	}

	Test_Assert_Always("Code buffer overflow exception not thrown.");
}

Case(ReadTemplated) {
	Gorc::Cog::VM::CodeBuffer cb;
	cb.Write<uint32_t>(0xFEEDBEEF, 5);
	Test_Assert_Eq(cb.Read<uint32_t>(5), 0xFEEDBEEF);
}

Case(ReadTemplatedFailure) {
	Gorc::Cog::VM::CodeBuffer cb;
	cb.Write<uint32_t>(0xFEEDBEEF, 4);
	try {
		cb.Read<uint32_t>(5);
	}
	catch(Gorc::Cog::VM::CodeBufferOverflowException&) {
		return;
	}

	Test_Assert_Always("Code buffer overflow exception not thrown.");
}

Case(ReadPointer) {
	Gorc::Cog::VM::CodeBuffer cb;
	cb.Write<uint32_t>(0xFEEDBEEF, 0);
	const void* rp = cb.ReadPointer(0, sizeof(uint32_t));

	uint32_t readValue = 0;
	memcpy(&readValue, rp, sizeof(uint32_t));

	Test_Assert_Eq(readValue, 0xFEEDBEEF);
}

Case(WritePointer) {
	Gorc::Cog::VM::CodeBuffer cb;
	void* wp = cb.WritePointer(0, sizeof(uint32_t));

	uint32_t writeValue = 0xFEEDBEEF;
	memcpy(wp, &writeValue, sizeof(uint32_t));

	Test_Assert_Eq(cb.Read<uint32_t>(0), writeValue);
}

Case(Size) {
	Gorc::Cog::VM::CodeBuffer cb;
	cb.Write<uint32_t>(0xFEEDBEEF, 0);
	cb.Write<uint32_t>(0xDEADBEEF, sizeof(uint32_t));
	cb.Write<uint32_t>(0xFEEDDEAD, sizeof(uint32_t) * 2);

	Test_Assert_Eq(cb.Size(), sizeof(uint32_t) * 3);
}

EndSuite(CodeBufferTests);

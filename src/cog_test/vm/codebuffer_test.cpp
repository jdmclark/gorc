#include "test/test.h"
#include "cog/vm/code_buffer.h"
#include "cog/vm/exception.h"

#include <string>
#include <cstring>

begin_suite(CodeBufferTests);

test_case(write) {
    gorc::cog::vm::code_buffer cb;

    std::string hello = "Hello, world!\n";

    cb.write(0, hello.c_str(), static_cast<size_t>(hello.length()));
    cb.write(96, hello.c_str(), static_cast<size_t>(hello.length()));
}

test_case(WriteTemplated) {
    gorc::cog::vm::code_buffer cb;
    cb.write<uint32_t>(0xFEEDBEEF, 5);
}

test_case(read) {
    gorc::cog::vm::code_buffer cb;

    uint32_t value = 0xFEEDBEEF;

    cb.write(0, &value, sizeof(uint32_t));

    uint32_t readvalue = 0;

    cb.read(&readvalue, 0, sizeof(uint32_t));

    assert_eq(readvalue, 0xFEEDBEEF);
}

test_case(ReadFailure) {
    gorc::cog::vm::code_buffer cb;

    uint32_t value = 0xFEEDBEEF;

    cb.write(0, &value, sizeof(uint32_t));

    try {
        uint32_t readvalue = 0;
        cb.read(&readvalue, 2, sizeof(uint32_t));
    }
    catch(gorc::cog::vm::code_buffer_overflow_exception&) {
        return;
    }

    assert_always("Code buffer overflow exception not thrown.");
}

test_case(ReadTemplated) {
    gorc::cog::vm::code_buffer cb;
    cb.write<uint32_t>(0xFEEDBEEF, 5);
    assert_eq(cb.read<uint32_t>(5), 0xFEEDBEEF);
}

test_case(ReadTemplatedFailure) {
    gorc::cog::vm::code_buffer cb;
    cb.write<uint32_t>(0xFEEDBEEF, 4);
    try {
        cb.read<uint32_t>(5);
    }
    catch(gorc::cog::vm::code_buffer_overflow_exception&) {
        return;
    }

    assert_always("Code buffer overflow exception not thrown.");
}

test_case(read_pointer) {
    gorc::cog::vm::code_buffer cb;
    cb.write<uint32_t>(0xFEEDBEEF, 0);
    const void* rp = cb.read_pointer(0, sizeof(uint32_t));

    uint32_t readvalue = 0;
    memcpy(&readvalue, rp, sizeof(uint32_t));

    assert_eq(readvalue, 0xFEEDBEEF);
}

test_case(write_pointer) {
    gorc::cog::vm::code_buffer cb;
    void* wp = cb.write_pointer(0, sizeof(uint32_t));

    uint32_t writevalue = 0xFEEDBEEF;
    memcpy(wp, &writevalue, sizeof(uint32_t));

    assert_eq(cb.read<uint32_t>(0), writevalue);
}

test_case(size) {
    gorc::cog::vm::code_buffer cb;
    cb.write<uint32_t>(0xFEEDBEEF, 0);
    cb.write<uint32_t>(0xDEADBEEF, sizeof(uint32_t));
    cb.write<uint32_t>(0xFEEDDEAD, sizeof(uint32_t) * 2);

    assert_eq(cb.size(), sizeof(uint32_t) * 3UL);
}

end_suite(CodeBufferTests);

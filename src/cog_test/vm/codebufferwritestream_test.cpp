#include "test/test.h"
#include "cog/vm/code_buffer_write_stream.h"
#include "cog/vm/code_buffer_read_stream.h"

begin_suite(CodeBufferWriteStreamTests);

test_case(seek) {
    gorc::cog::vm::code_buffer cBuffer;
    gorc::cog::vm::code_buffer_write_stream cb(cBuffer);

    cb.seek(128);
    assert_eq(cb.tell(), 128);
}

test_case(WriteStream) {
    gorc::cog::vm::code_buffer cBuffer;
    gorc::cog::vm::code_buffer_write_stream cb(cBuffer);

    std::string hello = "Hello, world!\n";

    cb.write(hello.c_str(), static_cast<size_t>(hello.length()));
    cb.write(hello.c_str(), static_cast<size_t>(hello.length()));
}

test_case(WriteStreamTemplated) {
    gorc::cog::vm::code_buffer cBuffer;
    gorc::cog::vm::code_buffer_write_stream cb(cBuffer);

    cb.write<uint32_t>(0xFEEDBEEF);
    cb.write<uint32_t>(0xABACADAB);
    cb.write<uint32_t>(0xF0F0F0F0);
}

test_case(write_buffer) {
    gorc::cog::vm::code_buffer cBufferOne;
    cBufferOne.write<uint32_t>(0xFEEDBEEF, 0);

    gorc::cog::vm::code_buffer cBuffer;
    gorc::cog::vm::code_buffer_write_stream cb(cBuffer);
    size_t destOffset = cb.write_buffer(cBufferOne);
    size_t destOffset2 = cb.write_buffer(cBufferOne);

    assert_eq(cBuffer.read<uint32_t>(0), 0xFEEDBEEF);
    assert_eq(cBuffer.read<uint32_t>(sizeof(uint32_t)), 0xFEEDBEEF);
    assert_eq(destOffset, 0);
    assert_eq(destOffset2, sizeof(uint32_t));
}

end_suite(CodeBufferWriteStreamTests);

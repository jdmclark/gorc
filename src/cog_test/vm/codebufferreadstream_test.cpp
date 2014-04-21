#include "test/test.h"
#include "cog/vm/code_buffer_read_stream.h"
#include "cog/vm/code_buffer_write_stream.h"

begin_suite(CodeBufferReadStreamTests);

test_case(seek) {
    gorc::cog::vm::code_buffer cBuffer;
    gorc::cog::vm::code_buffer_read_stream cb(cBuffer);

    cb.seek(128);
    assert_eq(cb.tell(), 128UL);
}

test_case(ReadStream) {
    gorc::cog::vm::code_buffer cBuffer;
    gorc::cog::vm::code_buffer_write_stream cb(cBuffer);
    gorc::cog::vm::code_buffer_read_stream db(cBuffer);

    uint32_t value = 0xFEEDBEEF;

    cb.write(&value, sizeof(uint32_t));

    uint32_t readvalue = 0;

    db.read(&readvalue, sizeof(uint32_t));

    assert_eq(readvalue, 0xFEEDBEEF);
}

test_case(ReadStreamTemplated) {
    gorc::cog::vm::code_buffer cBuffer;
    gorc::cog::vm::code_buffer_write_stream cb(cBuffer);
    gorc::cog::vm::code_buffer_read_stream db(cBuffer);

    cb.write<uint32_t>(0xFEEDBEEF);
    cb.write<uint32_t>(0xABACADAB);
    cb.write<uint32_t>(0xF0F0F0F0);

    assert_eq(db.read<uint32_t>(), 0xFEEDBEEF);
    assert_eq(db.read<uint32_t>(), 0xABACADAB);
    assert_eq(db.read<uint32_t>(), 0xF0F0F0F0);
}

end_suite(CodeBufferReadStreamTests);

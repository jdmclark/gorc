#include "test/test.h"
#include "base/io/memory_file.h"
#include "base/io/exception.h"
#include <vector>

using namespace gorc::io;

begin_suite(memory_file_test);

test_case(simple_read_write_buffer) {
    memory_file f;

    std::vector<int> inbuf { 5, -4, 584 };

    f.write(inbuf.data(), sizeof(int) * inbuf.size());

    std::vector<int> outbuf { 0, 0, 0 };

    f.set_position(0);
    f.read(outbuf.data(), sizeof(int) * inbuf.size());

    assert_eq(inbuf, outbuf);
}

test_case(read_write_offset) {
    memory_file f;

    write<unsigned char>(f, 0xFF);
    write<unsigned char>(f, 0xCD);
    write<unsigned char>(f, 0x9B);

    f.set_position(0);
    assert_eq(read<unsigned char>(f), 0xFF);

    f.set_position(2);
    assert_eq(read<unsigned char>(f), 0x9B);

    f.set_position(1);
    assert_eq(read<unsigned char>(f), 0xCD);
    assert_eq(read<unsigned char>(f), 0x9B);

    f.set_position(1);
    write<unsigned char>(f, 0xAA);
    assert_eq(read<unsigned char>(f), 0x9B);

    f.set_position(1);
    assert_eq(read<unsigned char>(f), 0xAA);
}

test_case(read_write_seek) {
    memory_file f;

    write<unsigned char>(f, 0xFF);
    write<unsigned char>(f, 0xCD);
    write<unsigned char>(f, 0x9B);

    f.seek(-3);
    assert_eq(read<unsigned char>(f), 0xFF);

    f.seek(1);
    assert_eq(read<unsigned char>(f), 0x9B);

    f.seek(-2);
    assert_eq(read<unsigned char>(f), 0xCD);
    f.seek(0);
    assert_eq(read<unsigned char>(f), 0x9B);

    f.seek(-2);
    write<unsigned char>(f, 0xAA);
    assert_eq(read<unsigned char>(f), 0x9B);

    f.seek(-2);
    assert_eq(read<unsigned char>(f), 0xAA);
}

test_case(out_of_bounds_set_pos) {
    memory_file f;

    for(int i = 0; i < 4; ++i) {
        write<unsigned char>(f, 0xFF);
    }

    f.set_position(0);
    f.set_position(1);
    f.set_position(2);
    f.set_position(3);
    f.set_position(4);

    try {
        f.set_position(5);
    }
    catch(const file_seek_exception&) {
        return;
    }

    assert_always("did not catch exception");
}

test_case(out_of_bounds_seek) {
    memory_file f;

    for(int i = 0; i < 4; ++i) {
        write<unsigned char>(f, 0xFF);
    }

    f.set_position(0);

    f.seek(1);
    f.seek(1);
    f.seek(1);
    f.seek(1);

    try {
        f.seek(1);
    }
    catch(const file_seek_exception&) {
        return;
    }

    assert_always("did not catch exception");
}

test_case(negative_seek) {
    memory_file f;

    for(int i = 0; i < 4; ++i) {
        write<unsigned char>(f, 0xFF);
    }

    f.seek(-1);
    f.seek(-1);
    f.seek(-1);
    f.seek(-1);

    try {
        f.seek(-1);
    }
    catch(const file_seek_exception&) {
        return;
    }

    assert_always("did not catch exception");
}

test_case(read_out_of_bounds) {
    memory_file f;

    for(int i = 0; i < 6; ++i) {
        write<unsigned char>(f, 0xFF);
    }

    f.set_position(0);
    assert_eq(read<uint32_t>(f), 0xFFFFFFFFU);

    try {
        read<uint32_t>(f);
    }
    catch(const file_read_exception&) {
        return;
    }

    assert_always("did not catch exception");
}

test_case(position_correct) {
    memory_file f;

    for(size_t i = 0; i < 6; ++i) {
        assert_eq(f.position(), i);
        write<unsigned char>(f, 0xFF);
    }

    assert_eq(f.position(), 6U);

    f.seek(0);
    assert_eq(f.position(), 6U);

    f.seek(-3);
    assert_eq(f.position(), 3U);

    f.set_position(1);
    assert_eq(f.position(), 1U);
}

test_case(size_correct) {
    memory_file f;

    for(size_t i = 0; i < 6; ++i) {
        assert_eq(f.size(), i);
        write<unsigned char>(f, 0xFF);
    }

    f.seek(-3);
    for(int i = 0; i < 3; ++i) {
        assert_eq(f.size(), 6U);
        write<unsigned char>(f, 0xAA);
    }

    for(size_t i = 0; i < 3; ++i) {
        assert_eq(f.size(), 6U + i);
        write<unsigned char>(f, 0xBB);
    }

    assert_eq(f.size(), 9U);
}

test_case(eof_correct) {
    memory_file f;

    for(int i = 0; i < 6; ++i) {
        assert_true(f.at_end());
        write<unsigned char>(f, 0xFF);
    }

    f.seek(-3);
    for(int i = 0; i < 3; ++i) {
        assert_true(!f.at_end());
        f.seek(1);
    }

    assert_true(f.at_end());
}

end_suite(memory_file_test);

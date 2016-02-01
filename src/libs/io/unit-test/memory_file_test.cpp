#include "test/test.hpp"
#include "io/memory_file.hpp"
#include <stdexcept>
#include <vector>

using namespace gorc;

namespace {
    template <typename StreamT>
    void write_char(StreamT &stream, unsigned char ch)
    {
        stream.write(&ch, sizeof(unsigned char));
    }

    template <typename StreamT>
    unsigned char read_char(StreamT &stream)
    {
        unsigned char ch;
        stream.read(&ch, sizeof(unsigned char));
        return ch;
    }
}

begin_suite(memory_file_test);

test_case(simple_read_write_buffer)
{
    memory_file f;

    std::vector<int> inbuf { 5, -4, 584 };

    f.write(inbuf.data(), sizeof(int) * inbuf.size());

    std::vector<int> outbuf { 0, 0, 0 };

    f.set_position(0);
    f.read(outbuf.data(), sizeof(int) * inbuf.size());

    assert_eq(inbuf, outbuf);
}

test_case(read_write_offset)
{
    memory_file f;

    write_char(f, 0xFF);
    write_char(f, 0xCD);
    write_char(f, 0x9B);

    f.set_position(0);
    assert_eq(read_char(f), 0xFF);

    f.set_position(2);
    assert_eq(read_char(f), 0x9B);

    f.set_position(1);
    assert_eq(read_char(f), 0xCD);
    assert_eq(read_char(f), 0x9B);

    auto w = memory_file::writer(f);
    w.set_position(1);
    write_char(w, 0xAB);

    auto r = memory_file::reader(f);
    r.set_position(1);
    assert_eq(read_char(r), 0xAB);
}

test_case(read_write_seek)
{
    memory_file f;

    write_char(f, 0xFF);
    write_char(f, 0xCD);
    write_char(f, 0x9B);

    assert_eq(read_char(f), 0xFF);

    f.seek(1);
    assert_eq(read_char(f), 0x9B);

    f.seek(-2);
    assert_eq(read_char(f), 0xCD);
    f.seek(0);
    assert_eq(read_char(f), 0x9B);
}

test_case(out_of_bounds_set_pos)
{
    memory_file f;

    for(int i = 0; i < 4; ++i) {
        write_char(f, 0xFF);
    }

    f.set_position(0);
    f.set_position(1);
    f.set_position(2);
    f.set_position(3);
    f.set_position(4);

    assert_throws(f.set_position(5),
                  std::runtime_error,
                  "memory_file::set_position invalid offset");
}

test_case(out_of_bounds_write_set_pos)
{
    memory_file f;

    for(int i = 0; i < 4; ++i) {
        write_char(f, 0xFF);
    }

    memory_file::writer w(f);

    w.set_position(0);
    w.set_position(1);
    w.set_position(2);
    w.set_position(3);
    w.set_position(4);

    assert_throws(w.set_position(5),
                  std::runtime_error,
                  "memory_file::set_position invalid offset");
}

test_case(out_of_bounds_seek)
{
    memory_file f;

    for(int i = 0; i < 4; ++i) {
        write_char(f, 0xFF);
    }

    f.set_position(0);

    f.seek(1);
    f.seek(1);
    f.seek(1);
    f.seek(1);

    try {
        f.seek(1);
    }
    catch(std::runtime_error const &e) {
        assert_eq(std::string(e.what()),
                  "memory_file::seek invalid offset");
        return;
    }

    assert_always("did not catch exception");
}

test_case(negative_seek)
{
    memory_file f;

    for(int i = 0; i < 4; ++i) {
        write_char(f, 0xFF);
    }

    try {
        f.seek(-1);
    }
    catch(std::runtime_error const &e) {
        assert_eq(std::string(e.what()),
                  "memory_file::seek invalid offset");
        return;
    }

    assert_always("did not catch exception");
}

test_case(read_out_of_bounds)
{
    memory_file f;

    for(int i = 0; i < 6; ++i) {
        write_char(f, 0xFF);
    }

    f.set_position(0);
    assert_eq(read<uint32_t>(f), 0xFFFFFFFFU);

    try {
        read<uint32_t>(f);
    }
    catch(std::runtime_error const &e) {
        assert_eq(std::string(e.what()),
                  "input_stream::read size exceeds bounds");
        return;
    }

    assert_always("did not catch exception");
}

test_case(position_correct)
{
    memory_file f;

    for(size_t i = 0; i < 6; ++i) {
        assert_eq(f.position(), 0U);
        write_char(f, 0xFF);
    }

    assert_eq(f.position(), 0U);

    f.seek(0);
    assert_eq(f.position(), 0U);

    f.seek(6);
    assert_eq(f.position(), 6U);

    f.seek(-3);
    assert_eq(f.position(), 3U);

    f.set_position(1);
    assert_eq(f.position(), 1U);
}

test_case(size_correct)
{
    memory_file f;

    for(size_t i = 0; i < 6; ++i) {
        assert_eq(f.size(), i);
        write_char(f, 0xFF);
    }

    assert_eq(f.size(), 6U);

    memory_file::writer f_writer(f);

    for(size_t i = 0; i < 9; ++i) {
        write_char(f_writer, 0xFF);
    }

    assert_eq(f.size(), 9U);
}

test_case(eof_correct)
{
    memory_file f;

    for(int i = 0; i < 6; ++i) {
        write_char(f, 0xFF);
    }

    for(int i = 0; i < 6; ++i) {
        assert_true(!f.at_end());
        f.seek(1);
    }

    assert_true(f.at_end());
}

test_case(write_some)
{
    memory_file f;

    std::string message = "Hello, World!";

    assert_eq(f.write_some(message.data(), message.size()), message.size());
}

test_case(read_some)
{
    memory_file f;

    std::string message = "Hello, World!";

    f.write(message.data(), message.size());

    char buffer[1024];

    assert_eq(f.read_some(buffer, 1024), message.size());

    assert_true(std::equal(message.begin(), message.end(), buffer));
}

test_case(copy)
{
    memory_file f;

    std::string message = "Hello, World!";

    f.write(message.data(), message.size());

    memory_file g;
    f.copy_to(g);
    g.set_position(0);

    std::string rdbuf;
    rdbuf.resize(message.size());
    g.read(&rdbuf[0], rdbuf.size());

    assert_eq(message, rdbuf);

    assert_true(g.at_end());
}

end_suite(memory_file_test);

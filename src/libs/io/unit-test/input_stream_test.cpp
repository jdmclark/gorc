#include "test/test.hpp"
#include "io/input_stream.hpp"
#include "io/memory_file.hpp"

using namespace gorc;

begin_suite(input_stream_test);

test_case(basic_io)
{
    memory_file f;
    for(int i = 0; i < 10; ++i) {
        f.write(&i, sizeof(int));
    }

    f.set_position(0);

    auto &non_interface_stream = f;
    for(int i = 0; i < 10; ++i) {
        assert_true(!non_interface_stream.at_end());
        int value;
        non_interface_stream.read(&value, sizeof(int));
        assert_eq(value, i);
    }

    assert_true(non_interface_stream.at_end());

    f.set_position(0);

    input_stream& stream = non_interface_stream;
    for(int i = 0; i < 10; ++i) {
        assert_true(!stream.at_end());
        int value;
        stream.read(&value, sizeof(int));
        assert_eq(value, i);
    }

    assert_true(stream.at_end());
}

test_case(copy_to)
{
    memory_file f;
    for(int i = 0; i < 10; ++i) {
        f.write(&i, sizeof(int));
    }

    memory_file::reader f_reader(f);

    memory_file g;
    f_reader.copy_to(g);

    for(int i = 0; i < 10; ++i) {
        int value;
        g.read(&value, sizeof(int));
        assert_eq(value, i);
    }
}

end_suite(input_stream_test);

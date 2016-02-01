#include "test/test.hpp"
#include "io/output_stream.hpp"
#include "io/memory_file.hpp"

using namespace gorc;

begin_suite(output_stream_test);

test_case(basic_io)
{
    memory_file f;

    auto &non_interface_stream = f;
    for(int i = 0; i < 10; ++i) {
        non_interface_stream.write(&i, sizeof(int));
    }

    f.set_position(0);
    for(int i = 0; i < 10; ++i) {
        int value;
        f.read(&value, sizeof(int));
        assert_eq(value, i);
    }

    assert_true(f.at_end());

    memory_file g;

    auto &non_interface_stream2 = g;

    output_stream& stream = non_interface_stream2;
    for(int i = 0; i < 10; ++i) {
        stream.write(&i, sizeof(int));
    }

    g.set_position(0);
    for(int i = 0; i < 10; ++i) {
        int value;
        g.read(&value, sizeof(int));
        assert_eq(value, i);
    }

    assert_true(g.at_end());
}

end_suite(output_stream_test);

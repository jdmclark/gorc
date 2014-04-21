#include "test/test.h"
#include "base/io/output_stream.h"
#include "base/io/memory_file.h"

using namespace gorc::io;

begin_suite(output_stream_test);

test_case(basic_io) {
    memory_file f;

    auto non_interface_stream = make_output_stream(f);
    for(int i = 0; i < 10; ++i) {
        write(non_interface_stream, i);
    }

    f.set_position(0);
    for(int i = 0; i < 10; ++i) {
        assert_eq(read<int>(f), i);
    }

    assert_true(f.at_end());

    memory_file g;

    auto non_interface_stream2 = make_output_stream(g);

    output_stream& stream = non_interface_stream2;
    for(int i = 0; i < 10; ++i) {
        write(stream, i);
    }

    g.set_position(0);
    for(int i = 0; i < 10; ++i) {
        assert_eq(read<int>(g), i);
    }

    assert_true(g.at_end());
}

end_suite(output_stream_test);

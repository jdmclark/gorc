#include "test/test.h"
#include "base/io/input_stream.h"
#include "base/io/memory_file.h"

using namespace gorc::io;

begin_suite(input_stream_test);

test_case(basic_io) {
    memory_file f;
    for(int i = 0; i < 10; ++i) {
        write(f, i);
    }

    f.set_position(0);

    auto non_interface_stream = make_input_stream(f);
    for(int i = 0; i < 10; ++i) {
        assert_true(!non_interface_stream.at_end());
        assert_eq(read<int>(non_interface_stream), i);
    }

    assert_true(non_interface_stream.at_end());

    f.set_position(0);

    input_stream& stream = non_interface_stream;
    for(int i = 0; i < 10; ++i) {
        assert_true(!stream.at_end());
        assert_eq(read<int>(stream), i);
    }

    assert_true(stream.at_end());
}

end_suite(input_stream_test);

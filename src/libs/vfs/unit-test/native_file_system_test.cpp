#include "test/test.hpp"
#include "vfs/native_file_system.hpp"

begin_suite(native_file_system_test);

test_case(simple)
{
    gorc::native_file_system nfs;

    auto f = nfs.open("nativefs/sample.txt");
    std::string msg;
    msg.resize(7);
    f->read(&msg[0], 7);
    assert_eq(msg, std::string("message"));
}

end_suite(native_file_system_test);

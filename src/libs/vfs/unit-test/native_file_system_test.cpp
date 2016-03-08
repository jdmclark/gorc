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

test_case(base_simple)
{
    gorc::native_file_system nfs("nativefs");

    auto f = nfs.open("sample.txt");
    std::string msg;
    msg.resize(7);
    f->read(&msg[0], 7);
    assert_eq(msg, std::string("message"));
}

test_case(prefix_simple)
{
    gorc::native_file_system nfs;

    auto f = nfs.find("sample.txt", { "does-not-exist", "nativefs" });

    assert_eq(std::get<0>(f), gorc::path("nativefs/sample.txt"));

    std::string msg;
    msg.resize(7);
    std::get<1>(f)->read(&msg[0], 7);
    assert_eq(msg, std::string("message"));
}

test_case(prefix_not_found)
{
    gorc::native_file_system nfs;

    assert_throws_logged(nfs.find("sample.txt", { "does_not_exist" }));
    assert_log_message(gorc::log_level::error,
                       "sample.txt: No such file or directory");
    assert_log_empty();
}

end_suite(native_file_system_test);

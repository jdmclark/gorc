#include "test/test.h"
#include "base/io/native_file_system.h"
#include "base/io/exception.h"
#include <cstring>

begin_suite(native_file_system_test);

test_case(exists) {
    gorc::io::native_file_system nfs("src/base_test/io/nfs_test_root");
}

test_case(open_existing_file) {
    gorc::io::native_file_system nfs("src/base_test/io/nfs_test_root");

    auto f = nfs.open("hello.txt");
    std::vector<char> hello;
    hello.resize(5);
    f->read(hello.data(), 5);
    hello.push_back('\0');

    assert_true(strcmp(hello.data(), "hello") == 0);

    auto g = nfs.open("subdir/world.txt");
    std::vector<char> world;
    world.resize(5);
    g->read(world.data(), 5);
    world.push_back('\0');

    assert_true(strcmp(world.data(), "world") == 0);
}

test_case(open_non_existing_file) {
    gorc::io::native_file_system nfs("src/base_test/io/nfs_test_root");

    try {
        auto f = nfs.open("foobar.tgz");
        assert_always("exception not thrown");
    }
    catch(const gorc::io::file_open_exception&) {
        // Success
    }
    catch(...) {
        assert_always("unknown exception thrown");
    }
}

end_suite(native_file_system_test);

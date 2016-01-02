#include "test/test.hpp"
#include "../parse_source_file.hpp"

using namespace gorc;

namespace {

    std::vector<path> default_search {
        "parse_source_file/search1",
        "parse_source_file/search2"
    };

}

begin_suite(parse_source_file_test);

test_case(file_not_found)
{
    auto res = get_source_file_headers("does_not_exist.cpp", { });
    assert_true(!res.has_value());

    assert_log_message(log_level::error, "Cannot open source file 'does_not_exist.cpp' for "
                                         "dependency analysis");
    assert_log_empty();
}

test_case(header_not_found)
{
    auto res = get_source_file_headers("parse_source_file/include_not_found.cpp", default_search);
    assert_true(!res.has_value());

    assert_log_message(log_level::error, "Cannot find header file 'baz.hpp' in search path");
    assert_log_empty();
}

test_case(system)
{
    auto res = get_source_file_headers("parse_source_file/include_system.cpp", default_search);
    assert_true(res.has_value());

    std::set<path> expected { };
    assert_eq(res.get_value(), expected);
}

test_case(relative)
{
    auto res = get_source_file_headers("parse_source_file/include_relative.cpp", default_search);
    assert_true(res.has_value());

    std::set<path> expected { "parse_source_file/foo.hpp" };
    assert_eq(res.get_value(), expected);
}

test_case(relative_child)
{
    auto res = get_source_file_headers("parse_source_file/include_relative_child.cpp",
                                       default_search);

    assert_true(res.has_value());

    std::set<path> expected { "parse_source_file/dir1/foo.hpp" };
    assert_eq(res.get_value(), expected);
}

test_case(denormal)
{
    auto res = get_source_file_headers("parse_source_file/dir1/include_denormal.cpp",
                                       default_search);

    assert_true(res.has_value());

    std::set<path> expected { "parse_source_file/dir2/foo.hpp" };
    assert_eq(res.get_value(), expected);
}

test_case(search)
{
    auto res = get_source_file_headers("parse_source_file/include_search.cpp",
                                       default_search);

    assert_true(res.has_value());

    std::set<path> expected { "parse_source_file/search1/bar.hpp" };
    assert_eq(res.get_value(), expected);
}

test_case(multiple)
{
    // Characteristic test
    auto res = get_source_file_headers("parse_source_file/include_multiple.cpp",
                                       default_search);

    assert_true(res.has_value());

    std::set<path> expected {
        "parse_source_file/foo.hpp",
        "parse_source_file/dir1/foo.hpp",
        "parse_source_file/dir2/foo.hpp",
        "parse_source_file/search1/bar.hpp"
    };

    assert_eq(res.get_value(), expected);
}

end_suite(parse_source_file_test);

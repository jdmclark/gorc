#include "test/test.hpp"
#include "utility/string_search.hpp"

using namespace gorc;

begin_suite(string_search_test);

test_case(begins_with_simple)
{
    assert_true(begins_with("asdf", "a"));
    assert_true(begins_with("asdf", "as"));
    assert_true(!begins_with("asdf", "s"));
    assert_true(!begins_with("asdf", "ase"));
}

test_case(begins_with_whole_string)
{
    assert_true(begins_with("asdf", "asdf"));
}

test_case(begins_with_degenerate)
{
    assert_true(begins_with("", ""));
}

test_case(begins_with_always_epsilon)
{
    assert_true(begins_with("asdf", ""));
}

test_case(begins_with_longer)
{
    assert_true(!begins_with("", "a"));
    assert_true(!begins_with("asdf", "asdfg"));
}

end_suite(string_search_test);

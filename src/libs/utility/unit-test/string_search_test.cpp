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

test_case(iequal_empty)
{
    assert_true(iequal(std::string(), std::string()));
}

test_case(iequal_one_empty)
{
    assert_true(!iequal(std::string("asdf"), std::string()));
    assert_true(!iequal(std::string(), std::string("asdf")));
}

test_case(iequal_simple)
{
    assert_true(iequal(std::string("asdf"), std::string("asdf")));
}

test_case(iequal_case_insensitive)
{
    assert_true(iequal(std::string("asdf"), std::string("AsDf")));
}

test_case(iequal_non_matching)
{
    assert_true(!iequal(std::string("asdf"), std::string("asgf")));
    assert_true(!iequal(std::string("asd"), std::string("asdf")));
    assert_true(!iequal(std::string("asdf"), std::string("asg")));
}

end_suite(string_search_test);

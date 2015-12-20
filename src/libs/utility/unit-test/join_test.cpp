#include "test/test.hpp"
#include "utility/join.hpp"

begin_suite(join_test);

test_case(empty_join_empty_string)
{
    std::vector<std::string> seq;

    assert_eq(gorc::join(seq, "something"), std::string());
}

test_case(single)
{
    std::vector<std::string> seq = { "a" };

    assert_eq(gorc::join(seq, "foo"), std::string("a"));
}

test_case(simple)
{
    std::vector<std::string> seq = { "a", "b", "c" };

    assert_eq(gorc::join(seq, "foo"), std::string("afoobfooc"));
}

end_suite(join_test);

#include "test/test.hpp"
#include "utility/strcat.hpp"

begin_suite(strcat_test);

test_case(empty_string)
{
    auto result = gorc::strcat();
    assert_true(result.empty());
}

test_case(one_string)
{
    auto result = gorc::strcat("test");
    assert_eq(result, std::string("test"));
}

test_case(format)
{
    auto result = gorc::strcat("some value: ", 5, " (", 10, ")");
    assert_eq(result, std::string("some value: 5 (10)"));
}

end_suite(strcat_test);

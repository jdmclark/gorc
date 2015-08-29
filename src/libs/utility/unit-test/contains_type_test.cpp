#include "test/test.hpp"
#include "utility/contains_type.hpp"

begin_suite(contains_type_test);

test_case(contains_type)
{
    assert_true((gorc::contains_type<int, int>::value));
    assert_true((gorc::contains_type<int, float, char, int, double>::value));
}

test_case(does_not_contain_type)
{
    assert_true(!(gorc::contains_type<int, char>::value));
    assert_true(!(gorc::contains_type<int, float, char, double, double>::value));
}

end_suite(contains_type_test);

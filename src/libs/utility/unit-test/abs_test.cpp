#include "test/test.hpp"
#include "utility/abs.hpp"

begin_suite(abs_test);

test_case(positive)
{
    assert_eq(gorc::abs(5), 5);
}

test_case(zero)
{
    assert_eq(gorc::abs(0), 0);
}

test_case(negative)
{
    assert_eq(gorc::abs(-5), 5);
}

end_suite(abs_test);

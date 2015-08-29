#include "test/test.hpp"
#include "utility/lcm.hpp"

begin_suite(lcm_test);

test_case(single)
{
    assert_eq(gorc::lcm(12), 12);
}

test_case(simple)
{
    assert_eq(gorc::lcm(3, 6), 6);
    assert_eq(gorc::lcm(6, 3), 6);
}

test_case(coprime)
{
    assert_eq(gorc::lcm(3, 4), 12);
    assert_eq(gorc::lcm(4, 3), 12);
}

test_case(variadic)
{
    assert_eq(gorc::lcm(2, 3, 4, 6, 12), 12);
}

end_suite(lcm_test);

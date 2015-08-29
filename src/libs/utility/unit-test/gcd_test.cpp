#include "test/test.hpp"
#include "utility/gcd.hpp"

begin_suite(gcd_test);

test_case(single)
{
    assert_eq(gorc::gcd(12), 12);
}

test_case(simple)
{
    assert_eq(gorc::gcd(20, 25), 5);
    assert_eq(gorc::gcd(25, 20), 5);
}

test_case(coprime)
{
    assert_eq(gorc::gcd(50, 51), 1);
    assert_eq(gorc::gcd(51, 50), 1);
}

test_case(variadic)
{
    assert_eq(gorc::gcd(5, 10, 15, 20, 25), 5);
}

end_suite(gcd_test);

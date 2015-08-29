#include "test/test.hpp"
#include "utility/mod.hpp"

begin_suite(mod_test);

test_case(simple)
{
    assert_eq(gorc::mod(5, 5), 0);
    assert_eq(gorc::mod(15, 2), 1);
}

test_case(negative)
{
    assert_eq(gorc::mod(-5, 3), 1);
}

end_suite(mod_test);

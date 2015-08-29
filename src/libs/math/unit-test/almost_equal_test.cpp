#include "test/test.hpp"
#include "math/almost_equal.hpp"

using namespace gorc;

begin_suite(almost_equal_test);

test_case(almost_equal_ints)
{
    assert_true(almost_equal(5, 5));
    assert_true(!almost_equal(5, 6));
}

test_case(almost_equal_floats)
{
    assert_true(almost_equal(5.0f, 5.0f));
    assert_true(!almost_equal(5.0f, 6.0f));
}

end_suite(almost_equal_test);

#include "test/test.hpp"
#include "math/point.hpp"

using namespace gorc;

begin_suite(point_test);

test_case(make_origin_point)
{
    assert_eq(make_point(0, 0, 0, 0), (make_origin_point<4, int>()));
}

end_suite(point_test);

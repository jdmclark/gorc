#include "test/test.hpp"
#include "math/size.hpp"

begin_suite(size_test);

test_case(volume_test)
{
    auto sz = gorc::make_size(2, 3, 5);
    assert_eq(gorc::volume(sz), 30);
}

test_case(degenerate_volume_test)
{
    auto sz = gorc::make_size(5, 10, 0);
    assert_eq(gorc::volume(sz), 0);
}

test_case(line_volume_test)
{
    auto sz = gorc::make_size(5);
    assert_eq(gorc::volume(sz), 5);
}

end_suite(size_test);

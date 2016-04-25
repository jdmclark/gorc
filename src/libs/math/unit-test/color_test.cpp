#include "test/test.hpp"
#include "math/color.hpp"
#include "math/almost_equal.hpp"

using namespace gorc;

begin_suite(color_test);

test_case(make_color_rgba32f)
{
    auto col = make_color(1.0f, 2.0f, 3.0f, 4.0f);
    assert_true(almost_equal(get<0>(col), 1.0f));
    assert_true(almost_equal(get<1>(col), 2.0f));
    assert_true(almost_equal(get<2>(col), 3.0f));
    assert_true(almost_equal(get<3>(col), 4.0f));
}

test_case(make_color_rgba32f_extend)
{
    auto col = make_color(1.0f, 2.0f, 3.0f);
    auto col2 = make_color(col, 4.0f);

    assert_true(almost_equal(get<0>(col2), 1.0f));
    assert_true(almost_equal(get<1>(col2), 2.0f));
    assert_true(almost_equal(get<2>(col2), 3.0f));
    assert_true(almost_equal(get<3>(col2), 4.0f));
}

test_case(make_color_rgb32f)
{
    auto col = make_color(1.0f, 2.0f, 3.0f);

    assert_true(almost_equal(get<0>(col), 1.0f));
    assert_true(almost_equal(get<1>(col), 2.0f));
    assert_true(almost_equal(get<2>(col), 3.0f));
}

test_case(make_color_rgba32f_solid)
{
    auto col = make_color(1.0f, 2.0f, 3.0f);
    auto col2 = solid(col);

    assert_true(almost_equal(get<0>(col2), 1.0f));
    assert_true(almost_equal(get<1>(col2), 2.0f));
    assert_true(almost_equal(get<2>(col2), 3.0f));
    assert_true(almost_equal(get<3>(col2), 1.0f));
}

test_case(make_color_rgba8)
{
    auto col = make_color_rgba8(32, 64, 128, 255);
    assert_eq(get<0>(col), 32);
    assert_eq(get<1>(col), 64);
    assert_eq(get<2>(col), 128);
    assert_eq(get<3>(col), 255);
}

test_case(make_color_rgba8_extend)
{
    auto col = make_color_rgb8(32, 64, 128);
    auto col2 = make_color_rgba8(col, 96);

    assert_eq(get<0>(col2), 32);
    assert_eq(get<1>(col2), 64);
    assert_eq(get<2>(col2), 128);
    assert_eq(get<3>(col2), 96);
}

test_case(make_color_rgb8)
{
    auto col = make_color_rgb8(32, 64, 128);
    assert_eq(get<0>(col), 32);
    assert_eq(get<1>(col), 64);
    assert_eq(get<2>(col), 128);
}

test_case(make_color_rgba8_solid)
{
    auto col = make_color_rgb8(32, 64, 128);
    auto col2 = solid(col);
    assert_eq(get<0>(col2), 32);
    assert_eq(get<1>(col2), 64);
    assert_eq(get<2>(col2), 128);
    assert_eq(get<3>(col2), 255);
}

end_suite(color_test);

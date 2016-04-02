#include "test/test.hpp"
#include "math/vector.hpp"

using namespace gorc;

begin_suite(vector_test);

test_case(vector_default_constructor)
{
    vector<4, int> v;
    for(auto c : v) {
        assert_eq(c, 0);
    }
}

test_case(vector_value_constructor)
{
    vector<4, int> v(1, 2, 3, 4);
    for(int i = 0; i < 4; ++i) {
        assert_eq(v.data[i], (i + 1));
    }
}

test_case(vector_iterators)
{
    vector<4, int> v;
    for(auto &c : v) {
        c = 12;
    }

    vector<4, int> const &w = v;
    for(auto const &c : w) {
        assert_eq(c, 12);
    }
}

test_case(make_vector)
{
    auto v = make_vector(1, 2, 3, 4);
    for(int i = 0; i < 4; ++i) {
        assert_eq(v.data[i], (i + 1));
    }
}

test_case(get)
{
    auto const v = make_vector(85, 48, 12);
    assert_eq(get<0>(v), 85);
    assert_eq(get<1>(v), 48);
    assert_eq(get<2>(v), 12);
}

test_case(get_set)
{
    vector<2, int> v;
    get<0>(v) = 12;
    get<1>(v) = 85;

    assert_eq(v.data[0], 12);
    assert_eq(v.data[1], 85);
}

test_case(almost_equal)
{
    auto v = make_vector(5, 10, 15);
    auto w = make_vector(5, 10, 15);

    assert_true(almost_equal(v, w));
}

test_case(almost_unequal)
{
    auto v = make_vector(5, 10, 15);

    auto v1 = make_vector(6, 10, 15);
    auto v2 = make_vector(5, 11, 15);
    auto v3 = make_vector(5, 10, 16);

    assert_true(!almost_equal(v, v1));
    assert_true(!almost_equal(v1, v));
    assert_true(!almost_equal(v, v2));
    assert_true(!almost_equal(v2, v));
    assert_true(!almost_equal(v, v3));
    assert_true(!almost_equal(v3, v));
}

test_case(add_assign)
{
    auto v = make_vector(5, 10, 15);
    auto w = make_vector(1, 2, 3);

    v += w;

    assert_eq(v, make_vector(6, 12, 18));
}

test_case(minus_assign)
{
    auto v = make_vector(5, 10, 15);
    auto w = make_vector(1, 2, 3);

    v -= w;

    assert_eq(v, make_vector(4, 8, 12));
}

test_case(scale_assign)
{
    auto v = make_vector(1, 2, 3);
    v *= 2;

    assert_eq(v, make_vector(2, 4, 6));
}

test_case(div_assign)
{
    auto v = make_vector(2, 4, 6);
    v /= 2;

    assert_eq(v, make_vector(1, 2, 3));
}

test_case(addition)
{
    auto v = make_vector(5, 10, 15);
    auto w = make_vector(1, 2, 3);

    assert_eq((v + w), make_vector(6, 12, 18));
}

test_case(subtraction)
{
    auto v = make_vector(5, 10, 15);
    auto w = make_vector(1, 2, 3);

    assert_eq((v - w), make_vector(4, 8, 12));
}

test_case(prefix_mul)
{
    auto v = make_vector(5, 10, 15);
    assert_eq((2 * v), make_vector(10, 20, 30));
}

test_case(postfix_mul)
{
    auto v = make_vector(5, 10, 15);
    assert_eq((v * 2), make_vector(10, 20, 30));
}

test_case(postfix_div)
{
    auto v = make_vector(10, 20, 30);
    assert_eq((v / 2), make_vector(5, 10, 15));
}

test_case(negate)
{
    auto v = make_vector(5, 10, 15);
    assert_eq((-v), make_vector(-5, -10, -15));
}

test_case(length_squared)
{
    auto v = make_vector(2, 3, 6);
    assert_eq(length_squared(v), 49);
}

test_case(length)
{
    auto v = make_vector(2, 3, 6);
    assert_eq(length(v), 7);
}

test_case(dot)
{
    auto v = make_vector(1, 2, 3);
    auto w = make_vector(3, 5, 7);
    assert_eq(dot(v, w), 34);
}

test_case(normalize)
{
    auto v = make_vector(0, 3, 0);
    assert_eq(normalize(v), make_vector(0, 1, 0));
}

test_case(cross)
{
    auto v = make_vector(5, 0, 0);
    auto w = make_vector(0, 2, 0);
    assert_eq(cross(v, w), make_vector(0, 0, 10));
}

end_suite(vector_test);

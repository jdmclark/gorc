#include "test/test.hpp"
#include "cog/script/value.hpp"

using namespace gorc;
using namespace gorc::cog;

begin_suite(value_test);

test_case(int_cast_to_float)
{
    value v(5);

    float v0 = v;

    assert_true(almost_equal(v0, 5.0f));
}

test_case(float_cast_to_int)
{
    value v(5.0f);

    int v0 = v;

    assert_eq(v0, 5);
}

test_case(id_cast_to_int)
{
    sector_id sid(28);

    value v(sid);

    int sid_val = v;

    assert_eq(sid_val, 28);
}

test_case(int_non_cast_to_id)
{
    value v(28);

    sector_id sid = v;

    assert_eq(sid, invalid_id);
}

test_case(int_operator_tests)
{
    value u(5);
    value v(3);

    assert_true(is_same(value(8), u + v));
    assert_true(is_same(value(2), u - v));
    assert_true(is_same(value(-5), -value(5)));
    assert_true(is_same(value(+5), +value(5)));
    assert_true(is_same(value(15), u * v));
    assert_true(is_same(value(1), u / v));
    assert_true(is_same(value(2), u % v));
    assert_true(is_same(value(7), u | v));
    assert_true(is_same(value(1), u & v));
    assert_true(is_same(value(6), u ^ v));

    assert_true((u == u));
    assert_true((u != v));
}

test_case(float_operator_tests)
{
    value u(5.0f);
    value v(3.0f);

    assert_true(is_same(value(8.0f), u + v));
    assert_true(is_same(value(2.0f), u - v));
    assert_true(is_same(value(-5.0f), -value(5.0f)));
    assert_true(is_same(value(+5.0f), +value(5.0f)));
    assert_true(is_same(value(15.0f), u * v));
    assert_true(is_same(value(5.0f / 3.0f), u / v));

    assert_true((u == u));
    assert_true((u != v));
}

test_case(bool_operator_tests)
{
    value u(true);
    value v(false);

    assert_true(is_same(u, !v));
    assert_true(is_same(v, !u));
    assert_true(is_same(u, u || v));
    assert_true(is_same(v, v || v));
    assert_true(is_same(u, u && u));
    assert_true(is_same(v, u && v));
}

test_case(cast_to_string)
{
    value u("Hello, World!");
    value v(5);

    assert_eq(std::string(static_cast<char const *>(u)), std::string("Hello, World!"));
    assert_eq(std::string(static_cast<char const *>(v)), std::string(""));
}

test_case(value_cast_to_bool)
{
    assert_true(static_cast<bool>(value(true)));
    assert_true(!static_cast<bool>(value(false)));

    assert_true(static_cast<bool>(value(5)));
    assert_true(!static_cast<bool>(value(0)));

    assert_true(static_cast<bool>(value(24.4f)));
    assert_true(!static_cast<bool>(value(0.0f)));

    assert_true(static_cast<bool>(value("Hello, World!")));
    assert_true(static_cast<bool>(value("")));

    assert_true(static_cast<bool>(value(make_vector(31.0f, 3.0f, 1.0f))));
    assert_true(static_cast<bool>(value(make_vector(0.0f, 0.0f, 0.0f))));

    assert_true(!static_cast<bool>(value(sector_id(58))));
}

test_case(cast_to_int)
{
    assert_eq(static_cast<int>(value(3)), 3);
    assert_eq(static_cast<int>(value(sector_id(85))), 85);
    assert_eq(static_cast<int>(value(3.0f)), 3);
    assert_eq(static_cast<int>(value(true)), 1);
    assert_eq(static_cast<int>(value(false)), 0);
    assert_eq(static_cast<int>(value(make_vector(1.0f, 2.0f, 1.0f))), -1);
}

test_case(cast_to_float)
{
    assert_true(almost_equal(static_cast<float>(value(3)), 3.0f));
    assert_true(almost_equal(static_cast<float>(value(sector_id(85))), 85.0f));
    assert_true(almost_equal(static_cast<float>(value(3.0f)), 3.0f));
    assert_true(almost_equal(static_cast<float>(value(true)), 1.0f));
    assert_true(almost_equal(static_cast<float>(value(false)), 0.0f));
    assert_true(almost_equal(static_cast<float>(value(make_vector(1.0f, 1.0f, 1.0f))), 0.0f));
}

test_case(cast_to_vector)
{
    assert_true(almost_equal(static_cast<vector<3, float>>(value(make_vector(1.0f, 2.0f, 3.0f))),
                             make_vector(1.0f, 2.0f, 3.0f)));
    assert_true(almost_equal(static_cast<vector<3, float>>(value(sector_id(583))),
                             make_vector(0.0f, 0.0f, 0.0f)));
}

test_case(is_same_type_mismatch)
{
    assert_true(!is_same(value(0), value(0.0f)));
}

test_case(is_same_string)
{
    assert_true(is_same(value("Hello, World!"), value("Hello, World!")));
    assert_true(!is_same(value("Hel|o, World!"), value("Hello, World!")));
}

test_case(is_same_vector)
{
    assert_true(is_same(value(make_vector(1.0f, 2.0f, 3.0f)),
                        value(make_vector(1.0f, 2.0f, 3.0f))));
    assert_true(!is_same(value(make_vector(1.0f, 2.0f, 3.0f)),
                         value(make_vector(1.0f, 2.0f, 3.1f))));
}

#define MAKE_ID_MEMBER_TEST(x) \
test_case(id_member_##x##_test) \
{ \
    value v(x##_id(5)); \
    value w(12.0f); \
    w = x##_id(5); \
    assert_true(is_same(v, w)); \
    x##_id xid = v; \
    assert_eq(xid, x##_id(5)); \
}

MAKE_ID_MEMBER_TEST(sector);
MAKE_ID_MEMBER_TEST(surface);
MAKE_ID_MEMBER_TEST(thing);
MAKE_ID_MEMBER_TEST(player);
MAKE_ID_MEMBER_TEST(ai);
MAKE_ID_MEMBER_TEST(cog);
MAKE_ID_MEMBER_TEST(colormap);
MAKE_ID_MEMBER_TEST(keyframe);
MAKE_ID_MEMBER_TEST(material);
MAKE_ID_MEMBER_TEST(model);
MAKE_ID_MEMBER_TEST(sound);
MAKE_ID_MEMBER_TEST(thing_template);
MAKE_ID_MEMBER_TEST(message);

end_suite(value_test);

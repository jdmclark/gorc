#include "test/test.hpp"
#include "cog/script/id.hpp"

using namespace gorc::cog;

begin_suite(id_test);

test_case(implicit_to_int)
{
    int expected = 5;
    thing_id value(5);

    int extracted = static_cast<int>(value);

    assert_eq(extracted, expected);
}

test_case(valid_comparison)
{
    thing_id a(5);
    thing_id b(5);
    thing_id c(7);

    assert_true(a == b);
    assert_true(b == a);
    assert_true(a != c);
    assert_true(c != a);
    assert_true(!(a != b));
    assert_true(!(b != a));
    assert_true(!(a == c));
    assert_true(!(c == a));
}

test_case(invalid_comparison)
{
    thing_id a(5);
    thing_id b(invalid_id);

    assert_ne(a, b);
    assert_ne(a, invalid_id);
    assert_ne(invalid_id, a);
    assert_eq(invalid_id, b);
    assert_eq(b, invalid_id);
}

test_case(default_invalid)
{
    thing_id a;
    assert_eq(a, invalid_id);
}

end_suite(id_test);

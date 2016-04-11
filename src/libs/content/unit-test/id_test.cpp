#include "test/test.hpp"
#include "content/id.hpp"

using namespace gorc;

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

test_case(at_id_non_const) {
    std::vector<int> values = { 1, 2, 3, 4, 5 };

    thing_id a(2);

    assert_eq(at_id(values, a), 3);

    at_id(values, a) = 5;

    assert_eq(values, (std::vector<int> { 1, 2, 5, 4, 5 }));
}

test_case(at_id_const) {
    std::vector<int> const values = { 1, 2, 3, 4, 5 };

    thing_id a(2);

    assert_eq(at_id(values, a), 3);
}

test_case(at_id_invalid) {
    std::vector<int> values = { 1, 2, 3 };

    thing_id a = invalid_id;

    assert_throws_logged(at_id(values, a));
    assert_log_message(log_level::error, "id -1 is out of bounds");
    assert_log_empty();
}

test_case(at_id_too_big) {
    std::vector<int> values = { 1, 2, 3 };

    thing_id a(3);

    assert_throws_logged(at_id(values, a));
    assert_log_message(log_level::error, "id 3 is out of bounds");
    assert_log_empty();
}

test_case(less) {
    thing_id a(3);
    thing_id b(3);
    thing_id c(2);
    thing_id d(4);

    assert_true(!(a < b));
    assert_true(!(a < c));
    assert_true(a < d);
}

test_case(lessequal) {
    thing_id a(3);
    thing_id b(3);
    thing_id c(2);
    thing_id d(4);

    assert_true(a <= b);
    assert_true(!(a <= c));
    assert_true(a <= d);
}

test_case(greater) {
    thing_id a(3);
    thing_id b(3);
    thing_id c(2);
    thing_id d(4);

    assert_true(!(a > b));
    assert_true(a > c);
    assert_true(!(a > d));
}

test_case(greaterequal) {
    thing_id a(3);
    thing_id b(3);
    thing_id c(2);
    thing_id d(4);

    assert_true(a >= b);
    assert_true(a >= c);
    assert_true(!(a >= d));
}

end_suite(id_test);

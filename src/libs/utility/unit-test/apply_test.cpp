#include "test/test.hpp"
#include "utility/apply.hpp"
#include "math/vector.hpp"

begin_suite(apply_test);

test_case(empty_tuple)
{
    auto fn = []() { return 5; };

    assert_eq(5, gorc::apply(fn, std::make_tuple()));
}

test_case(simple_tuple)
{
    bool called = false;

    auto fn = [&](int a, std::string b, int c) {
            called = true;
            assert_eq(a, 5);
            assert_eq(b, std::string("foo bar"));
            assert_eq(c, 10);
        };

    gorc::apply(fn, std::make_tuple(5, "foo bar", 10));

    assert_true(called);
}

test_case(vector_tuple)
{
    bool called = false;

    auto fn = [&](int a, int b, int c) {
            called = true;
            assert_eq(a, 5);
            assert_eq(b, 10);
            assert_eq(c, 15);
        };

    gorc::apply(fn, gorc::make_vector(5, 10, 15));

    assert_true(called);
}

end_suite(apply_test);

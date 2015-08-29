#include "test/test.hpp"
#include <exception>

begin_suite(assert_test);

test_case(base)
{
    try {
        assert_true(false);
    }
    catch(...) {
        assert_true(true);
        return;
    }

    assert_always("Assertion failed.");
}

test_case(always)
{
    try {
        assert_always("Test");
    }
    catch(...) {
        return;
    }

    assert_always("Assertion failed.");
}

test_case(eq)
{
    try {
        assert_eq(1, 0);
    }
    catch(...) {
        assert_eq(1, 1);
        return;
    }

    assert_always("Assertion failed.");
}

test_case(gt)
{
    try {
        assert_gt(0, 1);
    }
    catch(...) {
        assert_gt(1, 0);

        try {
            assert_gt(0, 0);
        }
        catch(...) {
            return;
        }
    }

    assert_always("Assertion failed.");
}

test_case(lt)
{
    try {
        assert_lt(1, 0);
    }
    catch(...) {
        assert_lt(0, 1);

        try {
            assert_lt(0, 0);
        }
        catch(...) {
            return;
        }
    }

    assert_always("Assertion failed.");
}

test_case(ge)
{
    try {
        assert_ge(0, 1);
    }
    catch(...) {
        assert_ge(1, 0);
        assert_ge(1, 1);
        return;
    }

    assert_always("Assertion failed.");
}

test_case(le)
{
    try {
        assert_le(1, 0);
    }
    catch(...) {
        assert_le(0, 1);
        assert_le(1, 1);
        return;
    }

    assert_always("Assertion failed.");
}

test_case(ne)
{
    try {
        assert_ne(1, 1);
    }
    catch(...) {
        assert_ne(1, 0);
        return;
    }

    assert_always("Assertion failed.");
}

test_case(throws)
{
    assert_throws(throw std::runtime_error("foo"),
                  std::runtime_error,
                  "foo");
}

test_case(no_throw)
{
    auto scratch = []() { return; };

    try {
        assert_throws(scratch(), std::runtime_error, "foo");
        assert_always("exception not thrown");
    }
    catch(test::exception const &e) {
        assert_eq(e.reason, "exception not thrown");
    }
}

test_case(throw_mismatch)
{
    try {
        assert_throws(throw std::runtime_error("bar"), std::runtime_error, "foo");
        assert_always("exception not thrown");
    }
    catch(test::exception const &e) {
        assert_eq(e.reason, "std::string(e.what())(bar) == (\"foo\")(foo) is false");
    }
}

test_case(throw_wrong_type)
{
    try {
        assert_throws(throw std::string("foo"), std::runtime_error, "foo");
        assert_always("exception not thrown");
    }
    catch(test::exception const &e) {
        assert_eq(e.reason, "incorrect exception type thrown");
    }
}

test_case(throw_wrong_type_std)
{
    try {
        assert_throws(throw std::domain_error("foo"), std::runtime_error, "foo");
        assert_always("exception not thrown");
    }
    catch(test::exception const &e) {
        assert_eq(e.reason, "incorrect exception type thrown: foo");
    }
}

end_suite(assert_test);

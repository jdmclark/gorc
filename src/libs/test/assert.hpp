#pragma once

#include "exception.hpp"
#include "strings.hpp"
#include "log/logged_runtime_error.hpp"

#define BALsX(x) #x
#define BALs(x) BALsX(x)
#define BAL BALs(__LINE__)

// Base assertion from which all others are defined.
#define base_assert(x, y)                                                           \
    do { if(!(x)) throw ::test::exception(__FILE__, __LINE__, y); } while(false)

// Base expected condition from which all others are defined.
#define base_expect(x, y)                                                           \
    do { if(!(x)) { ::test::test_passing = false;                                   \
    ::test::test_reporter->case_expectation_fail(::test::test_suite_name,           \
    ::test::test_case_name, y, __FILE__, __LINE__); } } while(false)

// Base relational assertion from which all others are defined.
#define base_rel_assert(x, y, z)                                                    \
    do {                                                                            \
        auto x_var = (x);                                                           \
        auto y_var = (y);                                                           \
        base_assert((x_var z y_var),                                                \
            ::test::strings::make_relational_assertion_string(                      \
                #x, x_var, #y, y_var, #z));                                         \
    } while(false)

// Base relational expectation from which all others are implemented.
#define base_rel_expect(x, y, z)                                                    \
    do {                                                                            \
        auto x_var = (x);                                                           \
        auto y_var = (y);                                                           \
        base_expect((x_var z y_var),                                                \
            ::test::strings::make_relational_assertion_string(                      \
                #x, x_var, #y, y_var, #z));                                         \
    } while(false)

// Simple assertion on a single expression
#define assert_true(x) base_assert((x), "Assertion failed")

// Simple expectation on a single expression
#define expect_true(x) base_expect((x), "Expectation failed")

// Assertion that always fails
#define assert_always(x) base_assert(false, x)

// Expectation that always fails
#define expect_always(x) base_expect(false, x)

// Assertion that fails unless both parameters are equal.
#define assert_eq(x, y) base_rel_assert(x, y, ==)

// Expectation that fails unless both parameters are equal.
#define expect_eq(x, y) base_rel_expect(x, y, ==)

// Assertion that fails unless the first parameter is greater than the second.
#define assert_gt(x, y) base_rel_assert(x, y, >)

// Expectation that fails unless the first parameter is greater than the second.
#define expect_gt(x, y) base_rel_expect(x, y, >)

// Assertion that fails unless the first parameter is less than the second.
#define assert_lt(x, y) base_rel_assert(x, y, <)

// Expectation that fails unless the first parameter is less than the second.
#define expect_lt(x, y) base_rel_expect(x, y, <)

// Assertion that fails unless the first parameter is greater than or equal to the second.
#define assert_ge(x, y) base_rel_assert(x, y, >=)

// Expectation that fails unless the first parameter is greater than or equal to the second.
#define expect_ge(x, y) base_rel_expect(x, y, >=)

// Assertion that fails unless the first parameter is less than or equal to the second.
#define assert_le(x, y) base_rel_assert(x, y, <=)

// Expectation that fails unless the first parameter is less than or equal to the second.
#define expect_le(x, y) base_rel_expect(x, y, <=)

// Assertion that fails unless both parameters are not equal.
#define assert_ne(x, y) base_rel_assert(x, y, !=)

// Expectation that fails unless both parameters are not equal.
#define expect_ne(x, y) base_rel_expect(x, y, !=)

// Assertion that fails unless the values of both ranges are equal.
#define assert_range_eq(x, y)                                                       \
    do {                                                                            \
        [](decltype(x) const &x_var, decltype(y) const &y_var) {                    \
            auto ix = x_var.begin();                                                \
            auto jx = x_var.end();                                                  \
            auto iy = y_var.begin();                                                \
            auto jy = y_var.end();                                                  \
            while(ix != jx && iy != jy) {                                           \
                assert_eq(*ix, *iy);                                                \
                ++ix;                                                               \
                ++iy;                                                               \
            }                                                                       \
            assert_eq(ix, jx);                                                      \
            assert_eq(iy, jy);                                                      \
        }(x, y);                                                                    \
    } while(false)

// Expectation that fails unless the values of both ranges are equal.
#define expect_range_eq(x, y)                                                       \
    do {                                                                            \
        [](decltype(x) const &x_var, decltype(y) const &y_var) {                    \
            auto ix = x_var.begin();                                                \
            auto jx = x_var.end();                                                  \
            auto iy = y_var.begin();                                                \
            auto jy = y_var.end();                                                  \
            while(ix != jx && iy != jy) {                                           \
                expect_eq(*ix, *iy);                                                \
                ++ix;                                                               \
                ++iy;                                                               \
            }                                                                       \
            expect_eq(ix, jx);                                                      \
            expect_eq(iy, jy);                                                      \
        }(x, y);                                                                    \
    } while(false)

// Assertion that fails if an exception is not thrown. Ignores contents.
#define assert_throws_type(expr, throw_type)                                        \
    do {                                                                            \
        try {                                                                       \
            (expr);                                                                 \
            assert_always("exception not thrown");                                  \
        }                                                                           \
        catch(throw_type const &e) {                                                \
            /* ignored */                                                           \
        }                                                                           \
        catch(test::exception const &) {                                            \
            throw;                                                                  \
        }                                                                           \
        catch(std::exception const &e) {                                            \
            assert_always(::gorc::strcat(                                           \
                "incorrect exception type thrown: ", e.what()));                    \
        }                                                                           \
        catch(...) {                                                                \
            assert_always("incorrect exception type thrown");                       \
        }                                                                           \
    } while(false)

// Assertion that fails if an exception is not thrown.
#define assert_throws(expr, throw_type, whatstr)                                    \
    do {                                                                            \
        try {                                                                       \
            (expr);                                                                 \
            assert_always("exception not thrown");                                  \
        }                                                                           \
        catch(throw_type const &e) {                                                \
            assert_eq(std::string(e.what()), (whatstr));                            \
        }                                                                           \
        catch(test::exception const &) {                                            \
            throw;                                                                  \
        }                                                                           \
        catch(std::exception const &e) {                                            \
            assert_always(::gorc::strcat(                                           \
                "incorrect exception type thrown: ", e.what()));                    \
        }                                                                           \
        catch(...) {                                                                \
            assert_always("incorrect exception type thrown");                       \
        }                                                                           \
    } while(false)

// Expectation that fails if an exception is not thrown.
#define expect_throws(expr, throw_type, whatstr)                                    \
    do {                                                                            \
        try {                                                                       \
            (expr);                                                                 \
            expect_always("exception not thrown");                                  \
        }                                                                           \
        catch(throw_type const &e) {                                                \
            expect_eq(std::string(e.what()), (whatstr));                            \
        }                                                                           \
        catch(test::exception const &) {                                            \
            throw;                                                                  \
        }                                                                           \
        catch(std::exception const &e) {                                            \
            expect_always(::gorc::strcat(                                           \
                "incorrect exception type thrown: ", e.what()));                    \
        }                                                                           \
        catch(...) {                                                                \
            expect_always("incorrect exception type thrown");                       \
        }                                                                           \
    } while(false)

// Assertion that fails if the next log message does not match.
#define assert_log_message(level, msg)                                              \
    do {                                                                            \
        assert_true(!log_message_queue.empty());                                    \
        assert_eq(std::get<0>(log_message_queue.front()), (level));                 \
        assert_eq(std::get<1>(log_message_queue.front()), (msg));                   \
        log_message_queue.pop();                                                    \
    } while(false)

// Assertion that fails if the log is not empty.
#define assert_log_empty()                                                          \
    assert_true(log_message_queue.empty())

// Assertion that fails if the logged_runtime_error exception is thrown.
#define assert_throws_logged(expr)                                                  \
    assert_throws((expr),                                                           \
                  ::gorc::logged_runtime_error,                                     \
                  "")

#pragma once

#include "t_exception.h"
#include "t_strings.h"
#include <boost/format.hpp>

#define BALsX(x) #x
#define BALs(x) BALsX(x)
#define BAL BALs(__LINE__)

/**
 * @brief Base assertion from which all others are implemented.
 * @param[in] x The expression to evaluate.
 * @param[in] y The message to display if x is false.
 */
#define base_assert(x, y)                                                             \
    do { if(!(x)) throw ::test::exception(__FILE__, __LINE__, y); } while(false)

/**
 * @brief Base expected condition from which all others are implemented.
 * @param[in] x The expression to evaluate.
 * @param[in] y The message to display if x is false.
 */
#define base_expect(x, y)                                                            \
    do { if(!(x)) { ::test::test_passing = false;                                    \
    ::test::test_reporter->case_expectation_fail(::test::test_suite_name,            \
    ::test::test_case_name, y, __FILE__, __LINE__); } } while(false)

/**
 * @brief Base relational assertion from which all others are implemented.
 * @param[in] x The first expression to evaluate.
 * @param[in] y The second expression to evaluate.
 * @param[in] z The relational operator to use.
 */
#define base_rel_assert(x, y, z)                                                     \
    do {                                                                             \
        auto x_var = (x);                                                            \
        auto y_var = (y);                                                            \
        base_assert((x_var z y_var),                                                 \
            ::test::strings::make_relational_assertion_string(                       \
                #x, x_var, #y, y_var, #z));                                          \
    } while(false)

/**
 * @brief Base relational expectation from which all others are implemented.
 * @param[in] x The first expression to evaluate.
 * @param[in] y The second expression to evaluate.
 * @param[in] z The relational operator to use.
 */
#define base_rel_expect(x, y, z)                                                     \
    do {                                                                             \
        auto x_var = (x);                                                            \
        auto y_var = (y);                                                            \
        base_expect((x_var z y_var),                                                 \
            ::test::strings::make_relational_assertion_string(                       \
                #x, x_var, #y, y_var, #z));                                          \
    } while(false)

/**
 * @brief Simple assertion on a single expression.
 * @param[in] x The expression to evaluate.
 */
#define assert_true(x) base_assert((x), "Assertion failed")

/**
 * @brief Simple expectation on a single expression.
 * @param[in] x The expression to evaluate.
 */
#define expect_true(x) base_expect((x), "Expectation failed")

/**
 * @brief Assertion that always fails.
 * @param[in] x The message to display.
 */
#define assert_always(x) base_assert(false, x)

/**
 * @brief Expectation that always fails.
 * @param[in] x The message to display.
 */
#define expect_always(x) base_expect(false, x)

/**
 * @brief Assertion that fails unless both parameters are equal.
 * @param[in] x The first parameter to test.
 * @param[in] y The second parameter to test.
 */
#define assert_eq(x, y) base_rel_assert(x, y, ==)

/**
 * @brief Expectation that fails unless both parameters are equal.
 * @param[in] x The first parameter to test.
 * @param[in] y The second parameter to test.
 */
#define expect_eq(x, y) base_rel_expect(x, y, ==)

/**
 * @brief Assertion that fails unless the first parameter is greater than the second.
 * @param[in] x The parameter that must be greater.
 * @param[in] y The parameter that must be less.
 */
#define assert_gt(x, y) base_rel_assert(x, y, >)

/**
 * @brief Expectation that fails unless the first parameter is greater than the second.
 * @param[in] x The parameter that must be greater.
 * @param[in] y The parameter that must be less.
 */
#define expect_gt(x, y) base_rel_expect(x, y, >)

/**
 * @brief Assertion that fails unless the first parameter is less than the second.
 * @param[in] x The parameter that must be less.
 * @param[in] y The parameter that must be greater.
 */
#define assert_lt(x, y) base_rel_assert(x, y, <)

/**
 * @brief Expectation that fails unless the first parameter is less than the second.
 * @param[in] x The parameter that must be less.
 * @param[in] y The parameter that must be greater.
 */
#define expect_lt(x, y) base_rel_expect(x, y, <)

/**
 * @brief Assertion that fails unless the first parameter is greater than or equal to the second.
 * @param[in] x The parameter that must be greater than or equal.
 * @param[in] y The parameter that must be less than or equal.
 */
#define assert_ge(x, y) base_rel_assert(x, y, >=)

/**
 * @brief Expectation that fails unless the first parameter is greater than or equal to the second.
 * @param[in] x The parameter that must be greater than or equal.
 * @param[in] y The parameter that must be less than or equal.
 */
#define expect_ge(x, y) base_rel_expect(x, y, >=)

/**
 * @brief Assertion that fails unless the first parameter is less than or equal to the second.
 * @param[in] x The parameter that must be less than or equal.
 * @param[in] y The parameter that must be greater than or equal.
 */
#define assert_le(x, y) base_rel_assert(x, y, <=)

/**
 * @brief Expectation that fails unless the first parameter is less than or equal to the second.
 * @param[in] x The parameter that must be less than or equal.
 * @param[in] y The parameter that must be greater than or equal.
 */
#define expect_le(x, y) base_rel_expect(x, y, <=)

/**
 * @brief Assertion that fails unless both parameters are not equal.
 * @param[in] x The first parameter to test.
 * @param[in] y The second parameter to test.
 */
#define assert_ne(x, y) base_rel_assert(x, y, !=)

/**
 * @brief Expectation that fails unless both parameters are not equal.
 * @param[in] x The first parameter to test.
 * @param[in] y The second parameter to test.
 */
#define expect_ne(x, y) base_rel_expect(x, y, !=)

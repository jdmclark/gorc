#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

// Framework includes:
#include "impl/t_singleton.h"
#include "impl/t_reporter.h"
#include "impl/t_case.h"
#include "impl/t_casefac.h"
#include "impl/t_casereg.h"
#include "impl/t_suite.h"
#include "impl/t_suitefac.h"
#include "impl/t_suitereg.h"
#include "impl/t_autorun.h"
#include "impl/t_fixture.h"
#include "impl/t_assert.h"

// Globals, needed to support assertions in subroutines and lambdas.
namespace test {
    // Test output reporter
    extern std::shared_ptr< ::test::reporter> test_reporter;

    // Name of the currently-executing suite.
    extern std::string test_suite_name;

    // Name of the currently-executing case.
    extern std::string test_case_name;

    // Pass/fail state of the current case.
    extern bool test_passing;
}

// Macro definitions:

/**
 * @brief Declares a new test suite using the specified fixture.
 * @param[in] x The name of the suite.
 * @param[in] y The typename of the fixture.
 */
#define begin_suite_fixture(x, y)                                    \
class x : public ::test::suite {                                     \
public:                                                              \
    x() {                                                            \
        ::test::lock_case_registry< test_fixture >();                \
    }                                                                \
                                                                     \
    class test_fixture : public y {                                  \
    public:                                                          \
        ~test_fixture() {}

/**
 * @brief Declares a new test suite.
 * @param[in] x The name of the suite.
 */
#define begin_suite(x) begin_suite_fixture(x, ::test::fixture)

/**
 * @brief Terminates the test suite.
 * @param[in] x The name of the suite.
 */
#define end_suite(x)                                                 \
};                                                                   \
::test::case_vector& get_case_factories() {                          \
    return ::test::case_registry<test_fixture>::get().factories;     \
}                                                                    \
::test::case_map& get_case_factory_map() {                           \
    return ::test::case_registry<test_fixture>::get().factory_map;   \
} };                                                                 \
::test::autorun< ::test::register_suite<x> > x##_aro(                \
    #x, __FILE__, __LINE__);

/**
 * @brief Defines a new test case.
 * @param[in] z The name of the case.
 */
#define test_case(z)                                                 \
};                                                                   \
class z##_case;                                                      \
struct z##_autoregister {                                            \
    z##_autoregister() {                                             \
        ::test::register_case< test_fixture, z##_case >(             \
            #z, __FILE__, __LINE__);                                 \
    }                                                                \
} z##_aro;                                                           \
class z##_case : public test_fixture {                               \
public:                                                              \
    void run()

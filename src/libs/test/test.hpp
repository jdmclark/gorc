#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

// Framework includes:
#include "reporter.hpp"
#include "case.hpp"
#include "case_factory.hpp"
#include "case_registry.hpp"
#include "suite.hpp"
#include "suite_factory.hpp"
#include "suite_registry.hpp"
#include "autorun.hpp"
#include "fixture.hpp"
#include "assert.hpp"

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

// Declares a new test suite using the specified fixture.
#define begin_suite_fixture(x, y)                                                       \
class x : public ::test::suite {                                                        \
public:                                                                                 \
    x()                                                                                 \
    {                                                                                   \
        ::test::lock_case_registry< test_fixture >();                                   \
    }                                                                                   \
                                                                                        \
    class test_fixture : public y {                                                     \
    public:                                                                             \
        ~test_fixture() {}

// Declares a new test suite using a default fixture.
#define begin_suite(x) begin_suite_fixture(x, ::test::fixture)

// Delimits the test suite.
#define end_suite(x)                                                                    \
    };                                                                                  \
                                                                                        \
    ::test::case_vector& get_case_factories()                                           \
    {                                                                                   \
        return ::gorc::get_global<::test::case_registry<test_fixture>>()->factories;    \
    }                                                                                   \
                                                                                        \
    ::test::case_map& get_case_factory_map()                                            \
    {                                                                                   \
        return ::gorc::get_global<::test::case_registry<test_fixture>>()->factory_map;  \
    }                                                                                   \
};                                                                                      \
::test::autorun< ::test::register_suite<x> > x##_aro( #x , __FILE__, __LINE__)

// Defines a new test case.
#define test_case(z)                                                                    \
    };                                                                                  \
                                                                                        \
    class z##_case;                                                                     \
                                                                                        \
    struct z##_autoregister {                                                           \
        z##_autoregister()                                                              \
        {                                                                               \
            ::test::register_case< test_fixture, z##_case >( #z , __FILE__, __LINE__);  \
        }                                                                               \
    } z##_aro;                                                                          \
                                                                                        \
    class z##_case : public test_fixture {                                              \
    public:                                                                             \
        void run()

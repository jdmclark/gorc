#pragma once

#include <type_traits>
#include <string>
#include "utility/strcat.hpp"
#include "utility/is_output_streamable.hpp"

namespace test {
    namespace strings {

        // Stream report printer messages
        extern std::string const unhandled_exception;
        extern std::string const suite_create_failure;
        extern std::string const fixture_setup_failure;
        extern std::string const fixture_teardown_failure;

        std::string make_total_tests_failed_string(int failcount);
        std::string make_total_tests_passed_string(int passcount);

        std::string make_case_failed_line_string(std::string const &filename,
                                                 int linenumber,
                                                 std::string const &suitename,
                                                 std::string const &casename,
                                                 std::string const &reason);
        std::string make_suite_failed_cases_string(std::string const &suitename,
                                                   int failcount);

        std::string make_unhandled_std_exception_string(std::string const &what);

        template <typename T,
                  typename IsT = typename ::gorc::is_output_streamable<T>>
        std::string conv(typename std::enable_if<IsT::value, T>::type const &v,
                         std::string const &name)
        {
            return ::gorc::strcat(name, "(", v, ")");
        }

        template <typename T>
        std::string conv(T const &, std::string const &name, ...)
        {
            return name;
        }

        template <typename Ta, typename Tb>
        std::string make_relational_assertion_string(std::string const &a_name,
                                                     Ta const &a_value,
                                                     std::string const &b_name,
                                                     Tb const &b_value,
                                                     std::string const &relop)
        {
            return ::gorc::strcat(conv<Ta>(a_value, a_name),
                                  " ",
                                  relop,
                                  " ",
                                  conv<Tb>(b_value, b_name),
                                  " is false");
        }

    }
}

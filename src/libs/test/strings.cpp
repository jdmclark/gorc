#include "strings.hpp"

std::string const test::strings::unhandled_exception = "Unhandled exception";
std::string const test::strings::suite_create_failure = "Creation failed";
std::string const test::strings::fixture_setup_failure = "Fixture setup failed";
std::string const test::strings::fixture_teardown_failure = "Fixture teardown failed";

std::string test::strings::make_total_tests_failed_string(int failcount)
{
    return ::gorc::strcat("Tests failed: ", failcount);
}

std::string test::strings::make_total_tests_passed_string(int passcount)
{
    return ::gorc::strcat("Test cases passed: ", passcount);
}

std::string test::strings::make_case_failed_line_string(const std::string& filename,
                                                        int linenumber,
                                                        const std::string& suitename,
                                                        const std::string& casename,
                                                        const std::string& reason)
{
    return ::gorc::strcat(filename,
                           "(",
                           linenumber,
                           "): ",
                           suitename,
                           "::",
                           casename,
                           " failed: ",
                           reason);
}

std::string test::strings::make_suite_failed_cases_string(const std::string& suitename,
                                                          int failcount)
{
    return ::gorc::strcat("Suite ", suitename, " failed: ", failcount, " cases");
}

std::string test::strings::make_unhandled_std_exception_string(const std::string& what)
{
    return ::gorc::strcat("Unhandled exception: ", what);
}

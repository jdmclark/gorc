#include <string>

#include "stream_reporter.hpp"
#include "strings.hpp"

test::stream_reporter::stream_reporter(std::ostream &s)
    : stream(s)
    , package_successes(0)
    , package_failures(0)
{
    return;
}

void test::stream_reporter::add_success(std::string const &suite_name)
{
    ++package_successes;
    ++suite_successes[suite_name];
}

void test::stream_reporter::add_failure(std::string const &suite_name)
{
    ++package_failures;
    ++suite_failures[suite_name];
}

void test::stream_reporter::report_begin(std::string const &)
{
    return;
}

void test::stream_reporter::report_end()
{
    stream << strings::make_total_tests_passed_string(package_successes) << std::endl;

    if(package_failures > 0) {
        stream << strings::make_total_tests_failed_string(package_failures) << std::endl;
    }
}

void test::stream_reporter::suite_begin(std::string const &)
{
    return;
}

void test::stream_reporter::suite_end(std::string const &suite_name)
{
    unsigned int s_failure_ct = suite_failures[suite_name];

    if(s_failure_ct > 0) {
        stream << strings::make_suite_failed_cases_string(suite_name, s_failure_ct) << std::endl;
    }
}

void test::stream_reporter::case_begin(std::string const &,
                                       std::string const &)
{
    return;
}

void test::stream_reporter::case_end(std::string const &,
                                     std::string const &)
{
    return;
}

void test::stream_reporter::case_pass(std::string const &suite_name,
                                      std::string const &)
{
    add_success(suite_name);
}

void test::stream_reporter::case_std_exception_fail(std::string const &suite_name,
                                                    std::string const &case_name,
                                                    std::string const &what,
                                                    std::string const &filename,
                                                    int line_number)
{
    add_failure(suite_name);

    auto reason = strings::make_unhandled_std_exception_string(what);
    auto fmt_reason = strings::make_case_failed_line_string(filename,
                                                            line_number,
                                                            suite_name,
                                                            case_name,
                                                            reason);

    stream << fmt_reason << std::endl;
}

void test::stream_reporter::case_unhandled_exception_fail(std::string const &suite_name,
                                                          std::string const &case_name,
                                                          std::string const &filename,
                                                          int line_number)
{
    add_failure(suite_name);
    stream << strings::make_case_failed_line_string(filename,
                                                    line_number,
                                                    suite_name,
                                                    case_name,
                                                    strings::unhandled_exception)
           << std::endl;
}

void test::stream_reporter::case_assertion_fail(std::string const &suite_name,
                                                std::string const &case_name,
                                                std::string const &reason,
                                                std::string const &filename,
                                                int line_number)
{
    add_failure(suite_name);
    stream << strings::make_case_failed_line_string(filename,
                                                    line_number,
                                                    suite_name,
                                                    case_name,
                                                    reason) << std::endl;
}

void test::stream_reporter::case_expectation_fail(std::string const &suite_name,
                                                  std::string const &case_name,
                                                  std::string const &reason,
                                                  std::string const &filename,
                                                  int line_number)
{
    add_failure(suite_name);
    stream << strings::make_case_failed_line_string(filename,
                                                    line_number,
                                                    suite_name,
                                                    case_name,
                                                    reason) << std::endl;
}

void test::stream_reporter::fixture_setup_fail(std::string const &suite_name,
                                               std::string const &case_name,
                                               std::string const &filename,
                                               int line_number)
{
    add_failure(suite_name);
    stream << strings::make_case_failed_line_string(filename,
                                                    line_number,
                                                    suite_name,
                                                    case_name,
                                                    strings::fixture_setup_failure) << std::endl;
}

void test::stream_reporter::fixture_teardown_fail(std::string const &suite_name,
                                                  std::string const &case_name,
                                                  std::string const &filename,
                                                  int line_number)
{
    add_failure(suite_name);
    stream << strings::make_case_failed_line_string(filename,
                                                    line_number,
                                                    suite_name,
                                                    case_name,
                                                    strings::fixture_teardown_failure) << std::endl;
}

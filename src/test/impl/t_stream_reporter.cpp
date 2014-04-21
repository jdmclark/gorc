#include <string>

#include "t_stream_reporter.h"
#include "t_strings.h"

test::stream_reporter::stream_reporter(std::ostream& s)
    : stream(s), package_successes(0), package_failures(0) {
    return;
}

void test::stream_reporter::add_success(const std::string& suite_name) {
    ++package_successes;
    ++suite_successes[suite_name];
}

void test::stream_reporter::add_failure(const std::string& suite_name) {
    ++package_failures;
    ++suite_failures[suite_name];
}

void test::stream_reporter::report_begin(const std::string& bin_name) {
    stream << strings::make_running_tests_string(bin_name) << std::endl;
}

void test::stream_reporter::report_end() {
    stream << strings::make_total_tests_passed_string(package_successes) << std::endl;

    if(package_failures > 0) {
        stream << strings::make_total_tests_failed_string(package_failures) << std::endl;
    }
}

void test::stream_reporter::suite_begin(const std::string&) {
    return;
}

void test::stream_reporter::suite_end(const std::string& suite_name) {
    unsigned int s_failure_ct = suite_failures[suite_name];

    if(s_failure_ct > 0) {
        stream << strings::make_suite_failed_cases_string(suite_name, s_failure_ct) << std::endl;
    }
}

void test::stream_reporter::case_begin(const std::string&, const std::string&) {
    return;
}

void test::stream_reporter::case_end(const std::string&, const std::string&) {
    return;
}

void test::stream_reporter::case_pass(const std::string& suite_name, const std::string&) {
    add_success(suite_name);
}

void test::stream_reporter::case_std_exception_fail(const std::string& suite_name, const std::string& case_name,
    const std::string& what, const std::string& filename, int lineNumber) {
    add_failure(suite_name);
    stream << strings::make_case_failed_line_string(filename, lineNumber, suite_name, case_name,
            strings::make_unhandled_std_exception_string(what)) << std::endl;
}

void test::stream_reporter::case_unhandled_exception_fail(const std::string& suite_name,
    const std::string& case_name, const std::string& filename, int lineNumber) {
    add_failure(suite_name);
    stream << strings::make_case_failed_line_string(filename, lineNumber, suite_name, case_name, strings::unhandled_exception) << std::endl;
}

void test::stream_reporter::case_assertion_fail(const std::string& suite_name, const std::string& case_name,
    const std::string& reason, const std::string& filename, int lineNumber) {
    add_failure(suite_name);
    stream << strings::make_case_failed_line_string(filename, lineNumber, suite_name, case_name, reason) << std::endl;
}

void test::stream_reporter::case_expectation_fail(const std::string& suite_name, const std::string& case_name,
    const std::string& reason, const std::string& filename, int lineNumber) {
    add_failure(suite_name);
    stream << strings::make_case_failed_line_string(filename, lineNumber, suite_name, case_name, reason) << std::endl;
}

void test::stream_reporter::fixture_setup_fail(const std::string& suite_name, const std::string& case_name,
    const std::string& filename, int lineNumber) {
    add_failure(suite_name);
    stream << strings::make_case_failed_line_string(filename, lineNumber, suite_name, case_name, strings::fixture_setup_failure) << std::endl;
}

void test::stream_reporter::fixture_teardown_fail(const std::string& suite_name, const std::string& case_name,
    const std::string& filename, int lineNumber) {
    add_failure(suite_name);
    stream << strings::make_case_failed_line_string(filename, lineNumber, suite_name, case_name, strings::fixture_teardown_failure) << std::endl;
}

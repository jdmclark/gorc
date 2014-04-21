#include "mock_reporter.h"

test_test::mock_reporter::mock_reporter() {
    report_begin_called = 0;
    report_end_called = 0;
    suite_begin_called = 0;
    suite_end_called = 0;
    case_begin_called = 0;
    case_end_called = 0;
    case_pass_called = 0;
    case_std_exception_fail_called = 0;
    case_unhandled_exception_fail_called = 0;
    case_assertion_fail_called = 0;
    case_expectation_fail_called = 0;
    fixture_setup_fail_called = 0;
    fixture_teardown_fail_called = 0;
}

test_test::mock_reporter::~mock_reporter() {
    return;
}

void test_test::mock_reporter::report_begin(const std::string&) {
    ++report_begin_called;
}

void test_test::mock_reporter::report_end() {
    ++report_end_called;
}

void test_test::mock_reporter::suite_begin(const std::string&) {
    ++suite_begin_called;
}

void test_test::mock_reporter::suite_end(const std::string&) {
    ++suite_end_called;
}

void test_test::mock_reporter::case_begin(const std::string&, const std::string&) {
    ++case_begin_called;
}

void test_test::mock_reporter::case_end(const std::string&, const std::string&) {
    ++case_end_called;
}

void test_test::mock_reporter::case_pass(const std::string&, const std::string&) {
    ++case_pass_called;
}

void test_test::mock_reporter::case_std_exception_fail(const std::string&, const std::string&,
    const std::string&, const std::string&, int) {
    ++case_std_exception_fail_called;
}

void test_test::mock_reporter::case_unhandled_exception_fail(const std::string&, const std::string&,
    const std::string&, int) {
    ++case_unhandled_exception_fail_called;
}

void test_test::mock_reporter::case_assertion_fail(const std::string&, const std::string&,
    const std::string&, const std::string&, int) {
    ++case_assertion_fail_called;
}

void test_test::mock_reporter::case_expectation_fail(const std::string&, const std::string&,
    const std::string&, const std::string&, int) {
    ++case_expectation_fail_called;
}

void test_test::mock_reporter::fixture_setup_fail(const std::string&, const std::string&,
    const std::string&, int) {
    ++fixture_setup_fail_called;
}

void test_test::mock_reporter::fixture_teardown_fail(const std::string&, const std::string&,
    const std::string&, int) {
    ++fixture_teardown_fail_called;
}

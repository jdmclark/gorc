#include "mock_reporter.h"

test_test::mock_reporter::mock_reporter() {
    list_begin_called = 0;
    list_end_called = 0;
    list_entry_suite_called = 0;
    list_entry_case_called = 0;
    report_begin_called = 0;
    report_end_called = 0;
    suite_begin_called = 0;
    suite_end_called = 0;
    case_begin_called = 0;
    case_end_called = 0;
    case_pass_called = 0;
    case_fail_called = 0;
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

void test_test::mock_reporter::list_begin() {
    ++list_begin_called;
}

void test_test::mock_reporter::list_end() {
    ++list_end_called;
}

void test_test::mock_reporter::list_entry_suite(const std::string& suite_name,
    const std::string& filename, int linenumber) {
    ++list_entry_suite_called;
}

void test_test::mock_reporter::list_entry_case(const std::string& suite_name, const std::string& case_name,
    const std::string& filename, int linenumber) {
    ++list_entry_case_called;
}

void test_test::mock_reporter::report_begin() {
    ++report_begin_called;
}

void test_test::mock_reporter::report_end() {
    ++report_end_called;
}

void test_test::mock_reporter::suite_begin(const std::string& suite_name) {
    ++suite_begin_called;
}

void test_test::mock_reporter::suite_end(const std::string& suite_name) {
    ++suite_end_called;
}

void test_test::mock_reporter::case_begin(const std::string& suite_name, const std::string& case_name) {
    ++case_begin_called;
}

void test_test::mock_reporter::case_end(const std::string& suite_name, const std::string& case_name) {
    ++case_end_called;
}

void test_test::mock_reporter::case_pass(const std::string& suite_name, const std::string& case_name) {
    ++case_pass_called;
}

void test_test::mock_reporter::case_fail(const std::string& suite_name, const std::string& case_name,
    const std::string& reason, const std::string& filename, int lineNumber) {
    ++case_fail_called;
}

void test_test::mock_reporter::case_std_exception_fail(const std::string& suite_name, const std::string& case_name,
    const std::string& what, const std::string& filename, int lineNumber) {
    ++case_std_exception_fail_called;
}

void test_test::mock_reporter::case_unhandled_exception_fail(const std::string& suite_name, const std::string& case_name,
    const std::string& filename, int lineNumber) {
    ++case_unhandled_exception_fail_called;
}

void test_test::mock_reporter::case_assertion_fail(const std::string& suite_name, const std::string& case_name,
    const std::string& reason, const std::string& filename, int lineNumber) {
    ++case_assertion_fail_called;
}

void test_test::mock_reporter::case_expectation_fail(const std::string& suite_name, const std::string& case_name,
    const std::string& reason, const std::string& filename, int lineNumber) {
    ++case_expectation_fail_called;
}

void test_test::mock_reporter::fixture_setup_fail(const std::string& suite_name, const std::string& case_name,
    const std::string& filename, int lineNumber) {
    ++fixture_setup_fail_called;
}

void test_test::mock_reporter::fixture_teardown_fail(const std::string& suite_name, const std::string& case_name,
    const std::string& filename, int lineNumber) {
    ++fixture_teardown_fail_called;
}

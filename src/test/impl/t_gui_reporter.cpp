#include <string>
#include <algorithm>

#include "t_gui_reporter.h"
#include "t_strings.h"

test::gui_reporter::gui_reporter(std::ostream& s) : stream(s) {
    return;
}

void test::gui_reporter::prepare_message(std::string& message) {
    std::replace(message.begin(), message.end(), '\n', ' ');
}

void test::gui_reporter::report_case_fail(const std::string& suite_name, const std::string& case_name,
    const std::string& reason, const std::string& filename, int lineNumber) {
    std::string preparedMessage(reason);
    prepare_message(preparedMessage);

    stream << "CASE_FAIL" << delim << suite_name << delim << case_name << delim << preparedMessage
        << delim << filename << delim << lineNumber << std::endl;
}

void test::gui_reporter::list_begin() {
    stream << "NULLUNIT" << std::endl;
    stream << "LIST_BEGIN" << std::endl;
}

void test::gui_reporter::list_end() {
    stream << "LIST_END" << std::endl;
}

void test::gui_reporter::list_entry_suite(const std::string& suite_name,
    const std::string& filename, int lineNumber) {
    stream << "LIST_SUITE" << delim << suite_name << delim << filename << delim << lineNumber << std::endl;
}

void test::gui_reporter::list_entry_case(const std::string& suite_name, const std::string& case_name,
    const std::string& filename, int lineNumber) {
    stream << "LIST_CASE" << delim << suite_name << delim << case_name << delim <<
        filename << delim << lineNumber << std::endl;
}

void test::gui_reporter::report_begin() {
    stream << "NULLUNIT" << std::endl;
    stream << "REPORT_BEGIN" << std::endl;
}

void test::gui_reporter::report_end() {
    stream << "REPORT_END" << std::endl;
}

void test::gui_reporter::suite_begin(const std::string& suite_name) {
    stream << "SUITE_BEGIN" << delim << suite_name << std::endl;
}

void test::gui_reporter::suite_end(const std::string& suite_name) {
    stream << "SUITE_END" << delim << suite_name << std::endl;
}

void test::gui_reporter::case_begin(const std::string& suite_name, const std::string& case_name) {
    stream << "CASE_BEGIN" << delim << suite_name << delim << case_name << std::endl;
}

void test::gui_reporter::case_end(const std::string& suite_name, const std::string& case_name) {
    stream << "CASE_END" << delim << suite_name << delim << case_name << std::endl;
}

void test::gui_reporter::case_pass(const std::string& suite_name, const std::string& case_name) {
    stream << "CASE_PASS" << delim << suite_name << delim << case_name << std::endl;
}

void test::gui_reporter::case_fail(const std::string& suite_name, const std::string& case_name,
    const std::string& reason, const std::string& filename, int lineNumber) {
    report_case_fail(suite_name, case_name, reason, filename, lineNumber);
}

void test::gui_reporter::case_std_exception_fail(const std::string& suite_name, const std::string& case_name,
    const std::string& what, const std::string& filename, int lineNumber) {
    report_case_fail(suite_name, case_name, strings::make_unhandled_std_exception_string(what), filename, lineNumber);
}

void test::gui_reporter::case_unhandled_exception_fail(const std::string& suite_name, const std::string& case_name,
    const std::string& filename, int lineNumber) {
    report_case_fail(suite_name, case_name, strings::unhandled_exception, filename, lineNumber);
}

void test::gui_reporter::case_assertion_fail(const std::string& suite_name, const std::string& case_name,
    const std::string& reason, const std::string& filename, int lineNumber) {
    report_case_fail(suite_name, case_name, reason, filename, lineNumber);
}

void test::gui_reporter::case_expectation_fail(const std::string& suite_name, const std::string& case_name,
    const std::string& reason, const std::string& filename, int lineNumber) {
    report_case_fail(suite_name, case_name, reason, filename, lineNumber);
}

void test::gui_reporter::fixture_setup_fail(const std::string& suite_name, const std::string& case_name,
    const std::string& filename, int lineNumber) {
    report_case_fail(suite_name, case_name, strings::fixture_setup_failure, filename, lineNumber);
}

void test::gui_reporter::fixture_teardown_fail(const std::string& suite_name, const std::string& case_name,
    const std::string& filename, int lineNumber) {
    report_case_fail(suite_name, case_name, strings::fixture_teardown_failure, filename, lineNumber);
}

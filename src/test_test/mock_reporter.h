#pragma once

#include "test/impl/t_reporter.h"

namespace test_test {

class mock_reporter : public test::reporter {
public:
    mock_reporter();
    virtual ~mock_reporter();

    int list_begin_called;
    int list_end_called;
    int list_entry_suite_called;
    int list_entry_case_called;
    int report_begin_called;
    int report_end_called;
    int suite_begin_called;
    int suite_end_called;
    int case_begin_called;
    int case_end_called;
    int case_pass_called;
    int case_fail_called;
    int case_std_exception_fail_called;
    int case_unhandled_exception_fail_called;
    int case_assertion_fail_called;
    int case_expectation_fail_called;
    int fixture_setup_fail_called;
    int fixture_teardown_fail_called;

    virtual void list_begin();
    virtual void list_end();

    virtual void list_entry_suite(const std::string& suite_name,
        const std::string& filename, int linenumber);
    virtual void list_entry_case(const std::string& suite_name, const std::string& case_name,
        const std::string& filename, int linenumber);

    virtual void report_begin();
    virtual void report_end();

    virtual void suite_begin(const std::string& suite_name);
    virtual void suite_end(const std::string& suite_name);

    virtual void case_begin(const std::string& suite_name, const std::string& case_name);
    virtual void case_end(const std::string& suite_name, const std::string& case_name);

    virtual void case_pass(const std::string& suite_name, const std::string& case_name);
    virtual void case_fail(const std::string& suite_name, const std::string& case_name,
        const std::string& reason, const std::string& filename, int lineNumber);
    virtual void case_std_exception_fail(const std::string& suite_name, const std::string& case_name,
        const std::string& what, const std::string& filename, int lineNumber);
    virtual void case_unhandled_exception_fail(const std::string& suite_name, const std::string& case_name,
        const std::string& filename, int lineNumber);
    virtual void case_assertion_fail(const std::string& suite_name, const std::string& case_name,
        const std::string& reason, const std::string& filename, int lineNumber);
    virtual void case_expectation_fail(const std::string& suite_name, const std::string& case_name,
        const std::string& reason, const std::string& filename, int lineNumber);

    virtual void fixture_setup_fail(const std::string& suite_name, const std::string& case_name,
        const std::string& filename, int lineNumber);
    virtual void fixture_teardown_fail(const std::string& suite_name, const std::string& case_name,
        const std::string& filename, int lineNumber);
};

}

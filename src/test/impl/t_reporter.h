#pragma once

#include <string>

namespace test {

class reporter {
public:
    virtual ~reporter() { }

    virtual void report_begin(const std::string& bin_name) = 0;
    virtual void report_end() = 0;

    virtual void suite_begin(const std::string& suite_name) = 0;
    virtual void suite_end(const std::string& suite_name) = 0;

    virtual void case_begin(const std::string& suite_name, const std::string& case_name) = 0;
    virtual void case_end(const std::string& suite_name, const std::string& case_name) = 0;

    virtual void case_pass(const std::string& suite_name, const std::string& case_name) = 0;
    virtual void case_std_exception_fail(const std::string& suite_name, const std::string& case_name,
        const std::string& what, const std::string& filename, int lineNumber) = 0;
    virtual void case_unhandled_exception_fail(const std::string& suite_name, const std::string& case_name,
        const std::string& filename, int lineNumber) = 0;
    virtual void case_assertion_fail(const std::string& suite_name, const std::string& case_name,
        const std::string& reason, const std::string& filename, int lineNumber) = 0;
    virtual void case_expectation_fail(const std::string& suite_name, const std::string& case_name,
        const std::string& reason, const std::string& filename, int lineNumber) = 0;

    virtual void fixture_setup_fail(const std::string& suite_name, const std::string& case_name,
        const std::string& filename, int lineNumber) = 0;
    virtual void fixture_teardown_fail(const std::string& suite_name, const std::string& case_name,
        const std::string& filename, int lineNumber) = 0;
};

}

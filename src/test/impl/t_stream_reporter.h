#pragma once

#include "t_reporter.h"

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <iostream>

namespace test {

class stream_reporter : public reporter {
    std::ostream& stream;

    std::unordered_map<std::string, unsigned int> suite_successes;
    std::unordered_map<std::string, unsigned int> suite_failures;

    unsigned int package_successes;
    unsigned int package_failures;

private:
    void add_success(const std::string& suite_name);
    void add_failure(const std::string& suite_name);

public:
    stream_reporter(std::ostream& s);

    void list_begin();
    void list_end();

    void list_entry_suite(const std::string& suite_name,
        const std::string& filename, int linenumber);
    void list_entry_case(const std::string& suite_name, const std::string& case_name,
        const std::string& filename, int linenumber);

    void report_begin();
    void report_end();

    void suite_begin(const std::string& suite_name);
    void suite_end(const std::string& suite_name);

    void case_begin(const std::string& suite_name, const std::string& case_name);
    void case_end(const std::string& suite_name, const std::string& case_name);

    void case_pass(const std::string& suite_name, const std::string& case_name);
    void case_fail(const std::string& suite_name, const std::string& case_name,
        const std::string& reason, const std::string& filename, int lineNumber);
    void case_std_exception_fail(const std::string& suite_name, const std::string& case_name,
        const std::string& what, const std::string& filename, int lineNumber);
    void case_unhandled_exception_fail(const std::string& suite_name, const std::string& case_name,
        const std::string& filename, int lineNumber);
    void case_assertion_fail(const std::string& suite_name, const std::string& case_name,
        const std::string& reason, const std::string& filename, int lineNumber);
    void case_expectation_fail(const std::string& suite_name, const std::string& case_name,
        const std::string& reason, const std::string& filename, int lineNumber);

    void fixture_setup_fail(const std::string& suite_name, const std::string& case_name,
        const std::string& filename, int lineNumber);
    void fixture_teardown_fail(const std::string& suite_name, const std::string& case_name,
        const std::string& filename, int lineNumber);
};

}

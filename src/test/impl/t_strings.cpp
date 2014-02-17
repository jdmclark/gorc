#include <sstream>
#include <string>
#include "t_strings.h"

std::string test::strings::make_illegal_argument_string(const std::string& arg) {
    std::stringstream ss;
    ss << "Error: Illegal argument " << arg;
    return ss.str();
}

std::string test::strings::make_invalid_suite_name_string(const std::string& suitename) {
    std::stringstream ss;
    ss << "Error: No suite exists with name \'" << suitename << "\'";
    return ss.str();
}

std::string test::strings::make_invalid_case_name_string(const std::string& suitename, const std::string& casename) {
    std::stringstream ss;
    ss << "Error: No case exists with name \'" << suitename << "." << casename << "\'";
    return ss.str();
}

std::string test::strings::make_total_tests_failed_string(int failcount) {
    std::stringstream ss;
    ss << "Tests failed: " << failcount;
    return ss.str();
}

std::string test::strings::make_total_tests_passed_string(int passcount) {
    std::stringstream ss;
    ss << "Test cases passed: " << passcount;
    return ss.str();
}

std::string test::strings::make_case_failed_line_string(const std::string& filename, int linenumber, const std::string& suitename,
        const std::string& casename, const std::string& reason) {
    std::stringstream ss;
    ss << filename << "(" << linenumber << "): " << suitename << "::" << casename << " failed: " << reason;
    return ss.str();
}

std::string test::strings::make_suite_failed_line_string(const std::string& filename, int linenumber, const std::string& suitename,
        const std::string& reason) {
    std::stringstream ss;
    ss << filename << "(" << linenumber << "): Suite " << suitename << " failed: " << reason;
    return ss.str();
}

std::string test::strings::make_suite_failed_cases_string(const std::string& suitename, int failcount) {
    std::stringstream ss;
    ss << "Suite " << suitename << " failed: " << failcount << " cases";
    return ss.str();
}

std::string test::strings::make_unhandled_std_exception_string(const std::string& what) {
    std::stringstream ss;
    ss << "Unhandled exception: " << what;
    return ss.str();
}

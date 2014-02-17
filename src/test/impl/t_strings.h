#pragma once

#include <type_traits>
#include <string>
#include <boost/format.hpp>
#include <boost/type_traits/has_left_shift.hpp>

namespace test {
namespace strings {

// Interface messages
static const char* multiple_output_types = "Error: Multiple output types selected";
static const char* case_before_suite = "Internal error: Case visited before parent suite";
static const char* running_tests = "Running unit tests";

// Stream report printer messages
static const char* unhandled_exception = "Unhandled exception";
static const char* suite_create_failure = "Creation failed";
static const char* fixture_setup_failure = "Fixture setup failed";
static const char* fixture_teardown_failure = "Fixture teardown failed";

std::string make_illegal_argument_string(const std::string& arg);
std::string make_invalid_suite_name_string(const std::string& suitename);
std::string make_invalid_case_name_string(const std::string& suitename, const std::string& casename);

std::string make_total_tests_failed_string(int failcount);
std::string make_total_tests_passed_string(int passcount);

std::string make_case_failed_line_string(const std::string& filename, int linenumber, const std::string& suitename, const std::string& casename, const std::string& reason);
std::string make_suite_failed_line_string(const std::string& filename, int linenumber, const std::string& suitename, const std::string& reason);
std::string make_suite_failed_cases_string(const std::string& suitename, int failcount);

std::string make_unhandled_std_exception_string(const std::string& what);

template <typename T>
std::string conv(const typename std::enable_if<boost::has_left_shift<std::ostream&, const T&>::value, T>::type& v) {
    return boost::str(boost::format("(%1%)") % v);
}

template <typename T>
std::string conv(const typename std::enable_if<!boost::has_left_shift<std::ostream&, const T&>::value, T>::type& v) {
    return std::string();
}

template <typename Ta, typename Tb>
std::string make_relational_assertion_string(const std::string& a_name, const Ta& a_value, const std::string& b_name, const Tb& b_value, const std::string& relop) {
    return boost::str(boost::format("%1%%2% %3% %4%%5% is false") % a_name % conv<Ta>(a_value) % relop % b_name % conv<Tb>(b_value));
}

}
}

#pragma once

#include <type_traits>
#include <string>
#include <boost/format.hpp>
#include <boost/type_traits/has_left_shift.hpp>

namespace test {
namespace strings {

// Stream report printer messages
static const char* unhandled_exception = "Unhandled exception";
static const char* suite_create_failure = "Creation failed";
static const char* fixture_setup_failure = "Fixture setup failed";
static const char* fixture_teardown_failure = "Fixture teardown failed";

std::string make_running_tests_string(const std::string& bin_name);

std::string make_total_tests_failed_string(int failcount);
std::string make_total_tests_passed_string(int passcount);

std::string make_case_failed_line_string(const std::string& filename, int linenumber, const std::string& suitename, const std::string& casename, const std::string& reason);
std::string make_suite_failed_cases_string(const std::string& suitename, int failcount);

std::string make_unhandled_std_exception_string(const std::string& what);

template <typename T>
std::string conv(const typename std::enable_if<boost::has_left_shift<std::ostream&, const T&>::value, T>::type& v, const std::string& name) {
    return boost::str(boost::format("%1%(%2%)") % name % v);
}

template <typename T>
std::string conv(const typename std::enable_if<!boost::has_left_shift<std::ostream&, const T&>::value, T>::type&, const std::string& name) {
    return boost::str(boost::format("%1%") % name);
}

template <typename Ta, typename Tb>
std::string make_relational_assertion_string(const std::string& a_name, const Ta& a_value, const std::string& b_name, const Tb& b_value, const std::string& relop) {
    return boost::str(boost::format("%1% %2% %3% is false") % conv<Ta>(a_value, a_name) % relop % conv<Tb>(b_value, b_name));
}

}
}

#include "mock_reporter.hpp"

mock_reporter::mock_reporter()
{
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

mock_reporter::~mock_reporter()
{
    return;
}

void mock_reporter::report_begin(std::string const &)
{
    ++report_begin_called;
}

void mock_reporter::report_end()
{
    ++report_end_called;
}

void mock_reporter::suite_begin(std::string const &)
{
    ++suite_begin_called;
}

void mock_reporter::suite_end(std::string const &)
{
    ++suite_end_called;
}

void mock_reporter::case_begin(std::string const &,
                               std::string const &)
{
    ++case_begin_called;
}

void mock_reporter::case_end(std::string const &,
                             std::string const &)
{
    ++case_end_called;
}

void mock_reporter::case_pass(std::string const &,
                              std::string const &)
{
    ++case_pass_called;
}

void mock_reporter::case_std_exception_fail(std::string const &,
                                            std::string const &,
                                            std::string const &,
                                            std::string const &,
                                            int)
{
    ++case_std_exception_fail_called;
}

void mock_reporter::case_unhandled_exception_fail(std::string const &,
                                                  std::string const &,
                                                  std::string const &,
                                                  int)
{
    ++case_unhandled_exception_fail_called;
}

void mock_reporter::case_assertion_fail(std::string const &,
                                        std::string const &,
                                        std::string const &,
                                        std::string const &,
                                        int)
{
    ++case_assertion_fail_called;
}

void mock_reporter::case_expectation_fail(std::string const &,
                                          std::string const &,
                                          std::string const &,
                                          std::string const &,
                                          int)
{
    ++case_expectation_fail_called;
}

void mock_reporter::fixture_setup_fail(std::string const &,
                                       std::string const &,
                                       std::string const &,
                                       int)
{
    ++fixture_setup_fail_called;
}

void mock_reporter::fixture_teardown_fail(std::string const &,
                                          std::string const &,
                                          std::string const &,
                                          int)
{
    ++fixture_teardown_fail_called;
}

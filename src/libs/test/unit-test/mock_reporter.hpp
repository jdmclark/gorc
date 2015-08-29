#pragma once

#include "test/reporter.hpp"

class mock_reporter : public test::reporter {
public:
    mock_reporter();
    virtual ~mock_reporter();

    int report_begin_called;
    int report_end_called;
    int suite_begin_called;
    int suite_end_called;
    int case_begin_called;
    int case_end_called;
    int case_pass_called;
    int case_std_exception_fail_called;
    int case_unhandled_exception_fail_called;
    int case_assertion_fail_called;
    int case_expectation_fail_called;
    int fixture_setup_fail_called;
    int fixture_teardown_fail_called;

    virtual void report_begin(std::string const &bin_name) override;
    virtual void report_end() override;

    virtual void suite_begin(std::string const &suite_name) override;
    virtual void suite_end(std::string const &suite_name) override;

    virtual void case_begin(std::string const &suite_name,
                            std::string const &case_name) override;
    virtual void case_end(std::string const &suite_name,
                          std::string const &case_name) override;

    virtual void case_pass(std::string const &suite_name,
                           std::string const &case_name) override;
    virtual void case_std_exception_fail(std::string const &suite_name,
                                         std::string const &case_name,
                                         std::string const &what,
                                         std::string const &filename,
                                         int line_number) override;
    virtual void case_unhandled_exception_fail(std::string const &suite_name,
                                               std::string const &case_name,
                                               std::string const &filename,
                                               int line_number) override;
    virtual void case_assertion_fail(std::string const &suite_name,
                                     std::string const &case_name,
                                     std::string const &reason,
                                     std::string const &filename,
                                     int line_number) override;
    virtual void case_expectation_fail(std::string const &suite_name,
                                       std::string const &case_name,
                                       std::string const &reason,
                                       std::string const &filename,
                                       int line_number) override;

    virtual void fixture_setup_fail(std::string const &suite_name,
                                    std::string const &case_name,
                                    std::string const &filename,
                                    int line_number) override;
    virtual void fixture_teardown_fail(std::string const &suite_name,
                                       std::string const &case_name,
                                       std::string const &filename,
                                       int line_number) override;
};

#pragma once

#include <string>

namespace test {

class reporter {
public:
    virtual ~reporter();

    virtual void report_begin(std::string const &bin_name) = 0;
    virtual void report_end() = 0;

    virtual void suite_begin(std::string const &suite_name) = 0;
    virtual void suite_end(std::string const &suite_name) = 0;

    virtual void case_begin(std::string const &suite_name,
                            std::string const &case_name) = 0;
    virtual void case_end(std::string const &suite_name,
                          std::string const &case_name) = 0;

    virtual void case_pass(std::string const &suite_name,
                           std::string const &case_name) = 0;
    virtual void case_std_exception_fail(std::string const &suite_name,
                                         std::string const &case_name,
                                         std::string const &what,
                                         std::string const &filename,
                                         int line_number) = 0;
    virtual void case_unhandled_exception_fail(std::string const &suite_name,
                                               std::string const &case_name,
                                               std::string const &filename,
                                               int line_number) = 0;
    virtual void case_assertion_fail(std::string const &suite_name,
                                     std::string const &case_name,
                                     std::string const &reason,
                                     std::string const &filename,
                                     int line_number) = 0;
    virtual void case_expectation_fail(std::string const &suite_name,
                                       std::string const &case_name,
                                       std::string const &reason,
                                       std::string const &filename,
                                       int line_number) = 0;

    virtual void fixture_setup_fail(std::string const &suite_name,
                                    std::string const &case_name,
                                    std::string const &filename,
                                    int line_number) = 0;
    virtual void fixture_teardown_fail(std::string const &suite_name,
                                       std::string const &case_name,
                                       std::string const &filename,
                                       int line_number) = 0;
};

}

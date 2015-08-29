#pragma once

#include "reporter.hpp"

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <iostream>

namespace test {

    class stream_reporter : public reporter {
        std::ostream &stream;

        std::unordered_map<std::string, unsigned int> suite_successes;
        std::unordered_map<std::string, unsigned int> suite_failures;

        unsigned int package_successes;
        unsigned int package_failures;

    private:
        void add_success(std::string const &suite_name);
        void add_failure(std::string const &suite_name);

    public:
        stream_reporter(std::ostream &s);

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

}

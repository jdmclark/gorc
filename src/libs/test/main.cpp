#include <string>

#include "test.hpp"
#include "strings.hpp"
#include "stream_reporter.hpp"

namespace test {

    std::shared_ptr<test::reporter> test_reporter;
    std::string test_suite_name;
    std::string test_case_name;
    bool test_passing;

    void run_case(int &failures,
                  std::string const &suite_name,
                  std::string const &case_name,
                  std::shared_ptr<case_factory> factory)
    {
        test_suite_name = suite_name;
        test_case_name = case_name;

        std::unique_ptr<case_object> test_case;

        // Setup case
        try {
            test_case = factory->create_case();
            test_case->setup();
        }
        catch(...) {
            test_reporter->fixture_setup_fail(suite_name,
                                              case_name,
                                              factory->filename,
                                              factory->line_number);
            ++failures;
            return;
        }

        // Run test case
        try {
            test_passing = true;
            test_case->run();

            if(test_passing) {
                test_reporter->case_pass(suite_name, case_name);
            }
            else {
                ++failures;
            }
        }
        catch(test::exception const &e) {
            test_reporter->case_assertion_fail(suite_name,
                                               case_name,
                                               e.reason,
                                               e.filename,
                                               e.line_number);
            ++failures;
        }
        catch(std::exception const &e) {
            test_reporter->case_std_exception_fail(suite_name,
                                                   case_name,
                                                   e.what(),
                                                   factory->filename,
                                                   factory->line_number);
            ++failures;
        }
        catch(...) {
            test_reporter->case_unhandled_exception_fail(suite_name,
                                                         case_name,
                                                         factory->filename,
                                                         factory->line_number);
            ++failures;
        }

        // Teardown case
        try {
            test_case->teardown();
            test_case.reset();
        }
        catch(...) {
            test_reporter->fixture_teardown_fail(suite_name,
                                                 case_name,
                                                 factory->filename,
                                                 factory->line_number);
            ++failures;
            return;
        }
    }

    void run_suite(int &failures,
                   std::string const &suite_name,
                   std::shared_ptr<suite_factory> factory)
    {
        std::shared_ptr<suite> suite = factory->create_suite();
        case_vector& caseFactories = suite->get_case_factories();

        for(auto jt = caseFactories.begin(); jt != caseFactories.end(); ++jt) {
            test_reporter->case_begin(suite_name, jt->first);
            run_case(failures, suite_name, jt->first, jt->second);
            test_reporter->case_end(suite_name, jt->first);
        }
    }

    void run_cases(std::string const &bin_name, int &failures)
    {
        test_reporter->report_begin(bin_name);

        auto &factories = ::gorc::get_global<suite_registry>()->factories;
        for(auto it = factories.begin(); it != factories.end(); ++it) {
            test_reporter->suite_begin(it->first);
            run_suite(failures, it->first, it->second);
            test_reporter->suite_end(it->first);
        }

        test_reporter->report_end();
    }

}

int main(int, char** argv) {
    // Build reporter
    test::test_reporter = std::make_shared<test::stream_reporter>(std::cout);

    int failures = 0;
    test::run_cases(argv[0], failures);

    return (failures == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

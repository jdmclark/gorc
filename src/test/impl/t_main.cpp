#include <string>

#include "test/test.h"
#include "t_strings.h"
#include "t_stream_reporter.h"

namespace test {

std::shared_ptr<test::reporter> test_reporter;
std::string test_suite_name;
std::string test_case_name;
bool test_passing;

void run_case(int& failures, const std::string& suiteName, const std::string& caseName, std::shared_ptr<case_factory> factory) {
    test_suite_name = suiteName;
    test_case_name = caseName;

    std::unique_ptr<case_object> testCase(nullptr);

    // Setup case
    try {
        testCase = std::unique_ptr<case_object>(factory->create_case());
        testCase->setup();
    }
    catch(...) {
        test_reporter->fixture_setup_fail(suiteName, caseName,
            factory->filename, factory->line_number);
        ++failures;
        return;
    }

    // Run test case
    try {
        test_passing = true;
        testCase->run();

        if(test_passing) {
            test_reporter->case_pass(suiteName, caseName);
        }
        else {
            ++failures;
        }
    }
    catch(const test::exception& e) {
        test_reporter->case_assertion_fail(suiteName, caseName,
            e.reason, e.filename, e.line_number);
        ++failures;
    }
    catch(const std::exception& e) {
        test_reporter->case_std_exception_fail(suiteName, caseName, e.what(),
            factory->filename, factory->line_number);
        ++failures;
    }
    catch(...) {
        test_reporter->case_unhandled_exception_fail(suiteName, caseName,
            factory->filename, factory->line_number);
        ++failures;
    }

    // Teardown case
    try {
        testCase->teardown();
        testCase = nullptr;
    }
    catch(...) {
        test_reporter->fixture_teardown_fail(suiteName, caseName,
            factory->filename, factory->line_number);
        ++failures;
        return;
    }
}

void run_suite(int& failures, const std::string& suiteName, std::shared_ptr<suite_factory> factory) {
    std::shared_ptr<suite> suite = factory->create_suite();
    case_vector& caseFactories = suite->get_case_factories();

    for(auto jt = caseFactories.begin(); jt != caseFactories.end(); ++jt) {
        test_reporter->case_begin(suiteName, jt->first);
        run_case(failures, suiteName, jt->first, jt->second);
        test_reporter->case_end(suiteName, jt->first);
    }
}

void run_cases(const std::string& bin_name, int& failures) {
    test_reporter->report_begin(bin_name);

    suite_map& factories = suite_registry::get().factories;
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

    return (failures == 0) ? 0 : 1;
}

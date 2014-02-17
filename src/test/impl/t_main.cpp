#include <string>

#include "test/test.h"
#include "t_strings.h"
#include "t_stream_reporter.h"
#include "t_gui_reporter.h"
#include "t_reporter_type.h"

namespace test {

std::shared_ptr<test::reporter> test_reporter;
std::string test_suite_name;
std::string test_case_name;
bool test_passing;

void list_case(const std::string& suiteName, const std::string& caseName, std::shared_ptr<case_factory> factory) {
    test_reporter->list_entry_case(suiteName, caseName, factory->filename, factory->line_number);
}

void list_suite(const std::string& suiteName, std::shared_ptr<suite_factory> factory) {
    test_reporter->list_entry_suite(suiteName, factory->filename, factory->line_number);

    std::shared_ptr<suite> suite = factory->create_suite();
    case_vector& caseFactories = suite->get_case_factories();

    for(auto jt = caseFactories.begin(); jt != caseFactories.end(); ++jt) {
        list_case(suiteName, jt->first, jt->second);
    }
}

void list_only_suite(int& failures, const std::string& suiteName, const std::string& caseName, std::shared_ptr<suite_factory> factory) {
    auto suite = factory->create_suite();
    auto caseFac = suite->get_case_factory_map().find(caseName);

    if(caseFac == suite->get_case_factory_map().end()) {
        ++failures;
        std::cerr << strings::make_invalid_case_name_string(suiteName, caseName) << std::endl;
    }
    else {
        list_case(suiteName, caseFac->first, caseFac->second);
    }
}

void list_cases() {
    test_reporter->list_begin();

    suite_map& factories = suite_registry::get().factories;
    for(auto it = factories.begin(); it != factories.end(); ++it) {
        list_suite(it->first, it->second);
    }

    test_reporter->list_end();
}

void list_only_cases(int& failures, const std::vector<std::pair<std::string, std::string>>& cases) {
    test_reporter->list_begin();

    for(auto it = cases.begin(); it != cases.end(); ++it) {
        auto suiteFac = suite_registry::get().factories.find(it->first);

        if(suiteFac == suite_registry::get().factories.end()) {
            ++failures;
            std::cerr << strings::make_invalid_suite_name_string(it->first) << std::endl;
        }
        else {
            if(it->second.empty()) {
                list_suite(suiteFac->first, suiteFac->second);
            }
            else {
                list_only_suite(failures, suiteFac->first, it->second, suiteFac->second);
            }
        }
    }

    test_reporter->list_end();
}

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

void run_only_suite(int& failures, const std::string& suiteName, const std::string& caseName, std::shared_ptr<suite_factory> factory) {
    auto suite = factory->create_suite();
    auto caseFac = suite->get_case_factory_map().find(caseName);

    if(caseFac == suite->get_case_factory_map().end()) {
        ++failures;
        std::cerr << strings::make_invalid_case_name_string(suiteName, caseName) << std::endl;
    }
    else {
        test_reporter->case_begin(suiteName, caseFac->first);
        run_case(failures, suiteName, caseFac->first, caseFac->second);
        test_reporter->case_end(suiteName, caseFac->first);
    }
}

void run_cases(int& failures) {
    test_reporter->report_begin();

    suite_map& factories = suite_registry::get().factories;
    for(auto it = factories.begin(); it != factories.end(); ++it) {
        test_reporter->suite_begin(it->first);
        run_suite(failures, it->first, it->second);
        test_reporter->suite_end(it->first);
    }

    test_reporter->report_end();
}

void run_only_cases(int& failures, const std::vector<std::pair<std::string, std::string>>& cases) {
    test_reporter->report_begin();

    for(auto it = cases.begin(); it != cases.end(); ++it) {
        auto suiteFac = suite_registry::get().factories.find(it->first);

        if(suiteFac == suite_registry::get().factories.end()) {
            ++failures;
            std::cerr << strings::make_invalid_suite_name_string(it->first) << std::endl;
        }
        else {
            if(it->second.empty()) {
                run_suite(failures, suiteFac->first, suiteFac->second);
            }
            else {
                run_only_suite(failures, suiteFac->first, it->second, suiteFac->second);
            }
        }
    }

    test_reporter->report_end();
}

void parse_case_name(const std::string& arg, std::pair<std::string, std::string>& pair) {
    auto it = arg.begin();

    for( ; it != arg.end(); ++it) {
        if(*it != '.') {
            pair.first.push_back(*it);
        }
        else {
            ++it;
            break;
        }
    }

    for( ; it != arg.end(); ++it) {
        pair.second.push_back(*it);
    }
}

}

int main(int argc, char** argv) {
    bool runOnly = false;
    bool listOnly = false;
    test::reporter_type reporterType = test::reporter_type::stream;
    std::vector<std::pair<std::string, std::string>> runOnlyCases;

    // Parse command line parameters
    for(int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);

        if(arg == "--list") {
            listOnly = true;
        }
        else if(arg == "--gui") {
            if(reporterType != test::reporter_type::stream) {
                std::cerr << test::strings::multiple_output_types << std::endl;
                return 1;
            }
            else {
                reporterType = test::reporter_type::gui;
            }
        }
        else if(arg == "--run") {
            runOnly = true;
            ++i;

            for( ; i < argc; ++i) {
                std::pair<std::string, std::string> parsed_arg;

                test::parse_case_name(argv[i], parsed_arg);

                runOnlyCases.push_back(parsed_arg);
            }
        }
        else {
            std::cerr << test::strings::make_illegal_argument_string(arg) << std::endl;
            return 1;
        }
    }

    // Build reporter
    switch(reporterType) {
    default:
    case test::reporter_type::stream:
        test::test_reporter = std::make_shared<test::stream_reporter>(std::cout);
        break;

    case test::reporter_type::gui:
        test::test_reporter = std::make_shared<test::gui_reporter>(std::cout);
        break;
    }

    int failures = 0;

    try {
        if(listOnly) {
            if(runOnly) {
                test::list_only_cases(failures, runOnlyCases);
            }
            else {
                test::list_cases();
            }
        }
        else {
            if(runOnly) {
                test::run_only_cases(failures, runOnlyCases);
            }
            else {
                test::run_cases(failures);
            }
        }
    }
    catch(const test::internal_exception& e) {
        std::cerr << e.reason << std::endl;
        return 1;
    }

    return (failures == 0) ? 0 : 1;
}

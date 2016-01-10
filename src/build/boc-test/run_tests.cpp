#include "run_tests.hpp"
#include "system/pipe.hpp"
#include "system/process.hpp"
#include "utility/progress.hpp"
#include "log/log.hpp"
#include <fstream>
#include <iostream>
#include <boost/filesystem.hpp>
#include <set>

int gorc::run_tests(std::set<path> const &tests,
                    path const &fail_log,
                    service_registry const &services,
                    bool print_summary)
{
    boost::filesystem::remove(fail_log);

    LOG_INFO(format("Running %d tests") % tests.size());

    auto progress = services.get<progress_factory>().make_progress(tests.size());

    std::set<path> passed_tests;
    std::set<path> failed_tests;

    for(auto const &test : tests) {
        std::vector<std::string> args {
            "scripts/run-test",
            test.native()
        };

        // TODO: don't suppress standard out/error this way
        pipe p;
        p.set_reusable(true);

        process test_proc("bash",
                          args,
                          nothing,
                          &p,
                          &p);

        // TODO: test timeout
        int result_code = test_proc.join();
        progress->advance();

        if(result_code == 0) {
            passed_tests.insert(test);
        }
        else {
            failed_tests.insert(test);
        }
    }

    progress->finished();

    if(print_summary) {
        if(!passed_tests.empty()) {
            std::cout << "Passed tests:" << std::endl;
            for(auto const &test : passed_tests) {
                std::cout << "    " << test.generic_string() << std::endl;
            }
        }

        if(!failed_tests.empty()) {
            std::cout << "Failed tests:" << std::endl;
            for(auto const &test : failed_tests) {
                std::cout << "    " << test.generic_string() << std::endl;
            }
        }
    }

    if(boost::filesystem::is_regular_file(fail_log)) {
        LOG_ERROR("Failed tests:");
        std::cout << std::endl << "Failed tests:" << std::endl;
        std::ifstream ifs(fail_log.native());
        if(ifs) {
            std::cerr << ifs.rdbuf();
            ifs.close();
        }

        return EXIT_FAILURE;
    }
    else {
        LOG_INFO("**** SUITE PASSED ****");
        return EXIT_SUCCESS;
    }
}

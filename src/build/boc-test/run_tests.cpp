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
                    path const &boc_shell,
                    path const &fail_log,
                    service_registry const &services,
                    bool print_summary)
{
    boost::filesystem::remove(fail_log);
    std::ofstream fail_log_ofs(fail_log.native());

    LOG_INFO(format("Running %d tests") % tests.size());

    auto progress = services.get<progress_factory>().make_progress(tests.size());

    std::set<path> passed_tests;
    std::set<path> failed_tests;

    for(auto const &test : tests) {
        boost::filesystem::remove_all(test / "tempdir");

        boost::filesystem::create_directories(test / "tempdir");
        auto p = gorc::make_output_file_pipe(test / "tempdir" / "test-log.txt");
        p.set_reusable(true);

        std::vector<std::string> args {
            "--script",
            "test.boc"
        };

        process test_proc(boc_shell,
                          args,
                          nothing,
                          &p,
                          &p,
                          test);

        // TODO: test timeout
        int result_code = test_proc.join();
        progress->advance();

        if(result_code == 0) {
            passed_tests.insert(test);
            boost::filesystem::remove_all(test / "tempdir");
        }
        else {
            failed_tests.insert(test);
            std::ifstream test_fail_log((test / "tempdir" / "test-log.txt").native());

            fail_log_ofs << std::endl
                         << "FAILED " << test.generic_string() << std::endl
                         << "--------------------" << std::endl
                         << test_fail_log.rdbuf();
        }
    }

    progress->finished();
    fail_log_ofs.close();

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

    if(!failed_tests.empty()) {
        LOG_ERROR("Failed tests:");
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

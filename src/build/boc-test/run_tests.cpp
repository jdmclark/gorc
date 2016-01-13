#include "run_tests.hpp"
#include "build/common/paths.hpp"
#include "system/pipe.hpp"
#include "system/process.hpp"
#include "utility/progress.hpp"
#include "log/log.hpp"
#include <fstream>
#include <iostream>
#include <boost/filesystem.hpp>
#include <set>
#include <thread>
#include <mutex>
#include <numeric>

int gorc::run_tests(std::set<path> const &tests,
                    path const &boc_shell,
                    path const &fail_log,
                    service_registry const &services,
                    size_t threads,
                    bool print_summary)
{
    boost::filesystem::remove(fail_log);
    std::ofstream fail_log_ofs(fail_log.native());

    LOG_INFO(format("Running %d tests") % tests.size());

    auto progress = services.get<progress_factory>().make_progress(tests.size());

    std::set<path> pending_tests = tests;
    std::set<path> passed_tests;
    std::set<path> failed_tests;
    std::mutex test_queue_lock;

    auto get_next_test = [&]() -> maybe<path> {
        std::lock_guard<std::mutex> lg(test_queue_lock);
        if(pending_tests.empty()) {
            return nothing;
        }

        auto it = pending_tests.begin();
        path rv = *it;
        pending_tests.erase(it);
        return rv;
    };

    auto retire_test = [&](path const &test, int result_code) {
        std::lock_guard<std::mutex> lg(test_queue_lock);
        progress->advance();

        if(result_code == 0) {
            passed_tests.insert(test);
            boost::filesystem::remove_all(test / boc_test_suite_dir);
        }
        else {
            failed_tests.insert(test);
            std::ifstream test_fail_log((test / boc_test_suite_dir / boc_test_log_filename).native());

            fail_log_ofs << std::endl
                         << "FAILED " << test.generic_string() << std::endl
                         << "--------------------" << std::endl
                         << test_fail_log.rdbuf();
        }
    };

    auto run_test = [&](path const &test) {
        boost::filesystem::remove_all(test / boc_test_suite_dir);

        boost::filesystem::create_directories(test / boc_test_suite_dir);
        auto p = gorc::make_output_file_pipe(test / boc_test_suite_dir / boc_test_log_filename);
        p.set_reusable(true);

        std::vector<std::string> args {
            "--script",
            boc_test_shell_filename.native()
        };

        process test_proc(boc_shell,
                          args,
                          nothing,
                          &p,
                          &p,
                          test);

        // TODO: test timeout
        return test_proc.join();
    };

    auto test_thread = [&]() {
        while(true) {
            auto next_test = get_next_test();
            if(!next_test.has_value()) {
                return;
            }

            int result_code = run_test(next_test.get_value());
            retire_test(next_test.get_value(), result_code);
        }
    };

    std::vector<std::thread> thread_pool;
    for(size_t i = 0; i < std::max(size_t(1), threads); ++i) {
        thread_pool.emplace_back(test_thread);
    }

    for(auto &thread : thread_pool) {
        thread.join();
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

        std::cerr << std::endl << std::endl;
        LOG_ERROR(format("%d tests failed") % failed_tests.size());

        return EXIT_FAILURE;
    }
    else {
        LOG_INFO("**** SUITE PASSED ****");
        return EXIT_SUCCESS;
    }
}

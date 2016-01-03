#include "run_tests.hpp"
#include "system/pipe.hpp"
#include "system/process.hpp"
#include <fstream>
#include <iostream>
#include <boost/filesystem.hpp>

int gorc::run_tests(std::set<path> const &tests, path const &fail_log)
{
    boost::filesystem::remove(fail_log);

    std::cout << "Running tests:" << std::endl;

    for(auto const &test : tests) {
        std::vector<std::string> args {
            "scripts/run-test",
            test.native()
        };

        process test_proc("bash",
                          args,
                          nothing,
                          nothing,
                          nothing);
        test_proc.join();
    }

    if(boost::filesystem::is_regular_file(fail_log)) {
        std::cout << std::endl << "Failed tests:" << std::endl;
        std::ifstream ifs(fail_log.native());
        if(ifs) {
            std::cout << ifs.rdbuf();
            ifs.close();
        }

        return EXIT_FAILURE;
    }
    else {
        std::cout << "**** SUITE PASSED ****" << std::endl;
        return EXIT_SUCCESS;
    }
}

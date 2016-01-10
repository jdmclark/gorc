#include "find_tests.hpp"
#include "utility/make_unique.hpp"
#include "utility/range.hpp"
#include "utility/join.hpp"
#include "log/log.hpp"
#include <memory>
#include <regex>
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

std::set<gorc::path> gorc::find_tests(std::vector<std::string> const &dirnames,
                                      path const &original_working_directory)
{
    std::regex assembled_regex(join(dirnames, "|"), std::regex::optimize);
    std::set<path> tests;
    std::set<path> test_roots;

    // Special case: working directory is a test
    if(std::regex_match(original_working_directory.filename().generic_string(), assembled_regex)) {
        LOG_INFO("Current directory is a single test");
        tests.insert(original_working_directory);
        return tests;
    }

    // Special case: working directory is the root
    if(equivalent(original_working_directory, ".")) {
        LOG_INFO("Current directory is project root");
        // TODO: Add more roots here.
        test_roots.insert("src");
    }
    else {
        LOG_INFO("Current directory is a test subset");
        test_roots.insert(original_working_directory);
    }

    // Working directory is not a test
    for(auto const &test_root : test_roots) {
        for(auto it = recursive_directory_iterator(test_root);
            it != recursive_directory_iterator();
            ++it) {
            if(std::regex_match(it->path().filename().generic_string(), assembled_regex)) {
                path test = it->path();
                tests.insert(test.normalize());

                // Do not recurse inside a matching test
                it.no_push();
            }
        }
    }

    return tests;
}

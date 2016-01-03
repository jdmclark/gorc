#include "find_tests.hpp"
#include "utility/make_unique.hpp"
#include "utility/range.hpp"
#include "utility/join.hpp"
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
        tests.insert(original_working_directory);
        return tests;
    }

    // Special case: working directory is the root
    if(equivalent(original_working_directory, ".")) {
        // TODO: Add more roots here.
        test_roots.insert("src");
    }
    else {
        test_roots.insert(original_working_directory);
    }

    // Working directory is not a test
    for(auto const &test_root : test_roots) {
        for(auto const &dir : make_range(recursive_directory_iterator(test_root),
                                         recursive_directory_iterator())) {
            if(std::regex_match(dir.path().filename().generic_string(), assembled_regex)) {
                tests.insert(dir.path());
            }
        }
    }

    return tests;
}

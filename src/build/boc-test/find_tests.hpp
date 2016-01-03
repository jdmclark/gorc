#pragma once

#include <vector>
#include <string>
#include <set>
#include "io/path.hpp"

namespace gorc {

    std::set<path> find_tests(std::vector<std::string> const &regexes,
                              path const &original_working_directory);

}

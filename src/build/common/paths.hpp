#pragma once

#include "io/path.hpp"
#include <string>
#include <vector>

namespace gorc {
    extern path const boc_project_filename;

    extern path const boc_test_log_filename;
    extern path const boc_test_suite_dir;
    extern path const boc_test_shell_filename;
    extern std::vector<std::string> const boc_test_default_directories;
    extern std::vector<std::string> const boc_test_default_roots;
}

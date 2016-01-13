#pragma once

#include "io/path.hpp"
#include <vector>
#include <string>

namespace gorc {

    extern path const boc_src_directory;
    extern path const boc_prog_filename;
    extern path const boc_lib_filename;
    extern path const boc_project_filename;
    extern path const boc_cache_filename;

    extern path const boc_test_log_filename;
    extern path const boc_test_suite_dir;
    extern path const boc_test_shell_filename;
    extern std::vector<std::string> const boc_test_default_directories;
    extern std::vector<std::string> const boc_test_default_roots;

}

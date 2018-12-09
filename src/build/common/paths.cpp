#include "paths.hpp"

gorc::path const gorc::boc_project_filename("project.json");

gorc::path const gorc::boc_test_log_filename("test-log.txt");
gorc::path const gorc::boc_test_suite_dir("tempdir");
gorc::path const gorc::boc_test_shell_filename("test.boc");

std::vector<std::string> const gorc::boc_test_default_directories{"^test-.*", "^unit-test$"};

std::vector<std::string> const gorc::boc_test_default_roots{"src"};

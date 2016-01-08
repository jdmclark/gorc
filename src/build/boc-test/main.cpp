#include "program/program.hpp"
#include "io/path.hpp"
#include "build/common/change_to_project_root.hpp"
#include "find_tests.hpp"
#include "run_tests.hpp"
#include "system/self.hpp"
#include "system/env.hpp"

namespace gorc {

    class boc_test_program : public program {
    public:
        std::vector<std::string> boc_test_directory {
            "^test-.*",
            "^unit-test$"
        };

        path original_working_directory;
        path project_root_path;
        path original_working_directory_rel;

        virtual void create_options(options &opts) override
        {
            // Test and debugging options
            opts.insert(make_multi_value_option("add-test-name",
                                                std::back_inserter(boc_test_directory)));
        }

        virtual int main() override
        {
            change_to_project_root(original_working_directory,
                                   project_root_path,
                                   original_working_directory_rel);

            auto tests = find_tests(boc_test_directory,
                                    original_working_directory_rel);

            // Find boc-shell relative to current executable
            path const &self_path = gorc::get_self_executable_path();
            path const &shell_path = self_path.parent_path() / "boc-shell";

            set_environment_variable("PLATFORM", "linux");
            set_environment_variable("FAILED_TEST_LOG",
                                     (project_root_path / "test-log.txt").native());
            set_environment_variable("PROJECT_ROOT", project_root_path.native());
            set_environment_variable("BOC_SHELL", shell_path.native());

            return run_tests(tests, "test-log.txt");
        }
    };

}

MAKE_MAIN(gorc::boc_test_program)

#include "program/program.hpp"
#include "io/path.hpp"
#include "build/common/change_to_project_root.hpp"
#include "find_tests.hpp"
#include "run_tests.hpp"
#include "system/self.hpp"
#include "system/env.hpp"
#include "build/common/paths.hpp"
#include "utility/service_registry.hpp"
#include "utility/shell_progress.hpp"
#include "utility/file_progress.hpp"
#include "log/log.hpp"

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

        bool no_progress = false;
        bool print_summary = false;
        size_t threads = 1;

        virtual void create_options(options &opts) override
        {
            // Test and debugging options
            opts.insert(make_multi_value_option("add-test-name",
                                                std::back_inserter(boc_test_directory)));

            opts.insert(make_switch_option("no-progress", no_progress));
            opts.insert(make_switch_option("print-summary", print_summary));

            opts.insert(make_value_option("threads", threads));
            opts.add_alias("threads", "-j");
        }

        virtual int main() override
        {
            LOG_INFO("Finding project root directory");
            change_to_project_root(original_working_directory,
                                   project_root_path,
                                   original_working_directory_rel);

            service_registry services;

            std::unique_ptr<progress_factory> prog_fac;
            if(no_progress) {
                prog_fac = make_unique<file_progress_factory>();
            }
            else {
                prog_fac = make_unique<shell_progress_factory>();
            }

            services.add<progress_factory>(*prog_fac);

            LOG_INFO("Finding tests");
            auto tests = find_tests(boc_test_directory,
                                    original_working_directory_rel);

            // Find boc-shell relative to current executable
            path const &self_path = gorc::get_self_executable_path();
            path const &shell_path = self_path.parent_path() / "boc-shell";

            set_environment_variable("PLATFORM", "linux");
            set_environment_variable("FAILED_TEST_LOG",
                                     (project_root_path / boc_test_log_filename).native());
            set_environment_variable("PROJECT_ROOT", project_root_path.native());
            set_environment_variable("BOC_SHELL", shell_path.native());

            return run_tests(tests,
                             shell_path,
                             boc_test_log_filename,
                             services,
                             threads,
                             print_summary);
        }
    };

}

MAKE_MAIN(gorc::boc_test_program)

#include "program/program.hpp"
#include "build/common/paths.hpp"
#include "build/common/change_to_project_root.hpp"
#include "build/common/make_progress_factory.hpp"
#include "find_tests.hpp"
#include "run_tests.hpp"
#include "system/self.hpp"
#include "system/env.hpp"
#include "utility/service_registry.hpp"
#include "log/log.hpp"

namespace gorc {

    class boc_test_program : public program {
    public:
        path original_working_directory;
        path project_root_path;
        path original_working_directory_rel;

        bool print_summary = false;
        size_t threads = 1;

        virtual void create_options(options &opts) override
        {
            opts.insert(make_switch_option("print-summary", print_summary));

            opts.insert(make_value_option("threads", threads));
            opts.add_alias("threads", "-j");
        }

        virtual int main() override
        {
            LOG_DEBUG("Finding project root directory");
            change_to_project_root(original_working_directory,
                                   project_root_path,
                                   original_working_directory_rel);

            service_registry services;

            std::unique_ptr<progress_factory> prog_fac = make_boc_progress_factory();
            services.add<progress_factory>(*prog_fac);

            LOG_DEBUG("Finding tests");
            auto tests = find_tests(boc_test_default_directories,
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

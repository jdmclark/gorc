#include "build/common/change_to_project_root.hpp"
#include "build/common/paths.hpp"
#include "program/program.hpp"
#include "subcommand_type.hpp"
#include "system/process.hpp"
#include "system/self.hpp"
#include "utility/join.hpp"
#include "utility/strcat.hpp"
#include <boost/filesystem.hpp>
#include <regex>
#include <vector>

using namespace boost::filesystem;

namespace gorc {

    class boc_frontend_program : public program {
    public:
        size_t threads = 1;
        std::string custom_build_type;

        std::vector<std::string> subcommands;

        virtual void create_options(options &opts) override
        {
            opts.insert_bare(make_bare_multi_value_option(std::back_inserter(subcommands)));

            opts.insert(make_value_option("threads", threads));
            opts.add_alias("threads", "-j");

            opts.insert(make_value_option("type", custom_build_type));
        }

        std::vector<std::string> construct_common_args() const
        {
            std::vector<std::string> rv;

            rv.push_back("-j");
            rv.push_back(std::to_string(threads));

            return rv;
        }

        virtual int run() override
        {
            if(subcommands.empty()) {
                return run_autodetect();
            }
            else {
                return run_sequence();
            }
        }

        int run_autodetect()
        {
            return run_test();
        }

        int run_sequence()
        {
            // Preconvert all targets. This will detect problems before running.
            std::vector<subcommand_type> subs;
            std::transform(subcommands.begin(),
                           subcommands.end(),
                           std::back_inserter(subs),
                           [](auto const &name) { return to_subcommand_type(name); });

            for(subcommand_type sub : subs) {
                int sub_result = EXIT_FAILURE;

                switch(sub) {
                case subcommand_type::test:
                    sub_result = run_test();
                    break;

                    // LCOV_EXCL_START
                    // Requires specialized configuration. Tested manually.
                case subcommand_type::coverage_report:
                    sub_result = run_coverage_report();
                    break;

                case subcommand_type::coveralls_coverage_report:
                    sub_result = run_coveralls_coverage_report();
                    break;
                }
                // LCOV_EXCL_STOP

                if(sub_result != EXIT_SUCCESS) {
                    return EXIT_FAILURE;
                }
            }

            return EXIT_SUCCESS;
        }

        int run_test()
        {
            path const &boc_test_path = get_self_executable_path().parent_path() / "boc-test";

            auto args = construct_common_args();

            process test_proc(boc_test_path,
                              args,
                              /* stdin */ nothing,
                              /* stdout */ nothing,
                              /* stderr */ nothing,
                              /* cwd */ nothing);

            return test_proc.join();
        }

        class scoped_project_root {
        private:
            path original_path;

        public:
            scoped_project_root()
                : original_path(current_path())
            {
                path junk1, junk2, junk3;
                change_to_project_root(junk1, junk2, junk3);
                return;
            }

            ~scoped_project_root()
            {
                current_path(original_path);
            }
        };

        // LCOV_EXCL_START
        // Coverage requires a specialized configuration. Tested manually.
        int run_coverage_report()
        {
            LOG_INFO("Generating coverage report");
            scoped_project_root srp;

            std::vector<std::string> args{"--root",
                                          canonical(current_path()).generic_string(),
                                          "--object-directory",
                                          canonical(current_path()).generic_string(),
                                          "--sort-uncovered",
                                          "--exclude-unreachable-branches"};

            process report_proc("gcovr",
                                args,
                                /* stdin */ nothing,
                                /* stdout */ nothing,
                                /* stderr */ nothing,
                                /* cwd */ nothing);

            return report_proc.join();
        }

        int run_coveralls_coverage_report()
        {
            LOG_INFO("Generating coverage report");
            scoped_project_root srp;

            std::vector<std::string> args{"--gcov-options",
                                          "\\-lp",
                                          "--root",
                                          canonical(current_path()).generic_string(),
                                          "--build-root",
                                          canonical(current_path()).generic_string()};

            process report_proc("cpp-coveralls",
                                args,
                                /* stdin */ nothing,
                                /* stdout */ nothing,
                                /* stderr */ nothing,
                                /* cwd */ nothing);

            return report_proc.join();
        }
        // LCOV_EXCL_STOP
    };
}

MAKE_MAIN(gorc::boc_frontend_program)

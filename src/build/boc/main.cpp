#include "program/program.hpp"
#include "build/common/paths.hpp"
#include "utility/join.hpp"
#include "system/self.hpp"
#include "system/process.hpp"
#include "subcommand_type.hpp"
#include "build/common/change_to_project_root.hpp"
#include "build/common/project_file.hpp"
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

        virtual int main() override
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
            auto cwd = canonical(current_path());

            std::regex assembled_testname_regex(join(boc_test_default_directories, "|"));
            if(std::regex_match(cwd.filename().generic_string(), assembled_testname_regex)) {
                // Current directory is a test name
                return run_test();
            }
            else {
                return run_build();
            }
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
                case subcommand_type::build:
                    sub_result = run_build();
                    break;

                case subcommand_type::test:
                    sub_result = run_test();
                    break;

                case subcommand_type::clean:
                    sub_result = run_clean();
                    break;

                case subcommand_type::coverage_report:
                    sub_result = run_coverage_report();
                    break;

                case subcommand_type::coveralls_coverage_report:
                    sub_result = run_coveralls_coverage_report();
                    break;
                }

                if(sub_result != EXIT_SUCCESS) {
                    return EXIT_FAILURE;
                }
            }

            return EXIT_SUCCESS;
        }

        int run_test()
        {
            path const &boc_test_path =
                get_self_executable_path().parent_path() / "boc-test";

            auto args = construct_common_args();

            process test_proc(boc_test_path,
                              args,
                              /* stdin */ nothing,
                              /* stdout */ nothing,
                              /* stderr */ nothing,
                              /* cwd */ nothing);

            return test_proc.join();
        }

        int run_build()
        {
            path const &boc_build_path =
                get_self_executable_path().parent_path() / "boc-build";

            auto args = construct_common_args();

            if(!custom_build_type.empty()) {
                args.push_back("--type");
                args.push_back(custom_build_type);
            }

            process build_proc(boc_build_path,
                               args,
                               /* stdin */ nothing,
                               /* stdout */ nothing,
                               /* stderr */ nothing,
                               /* cwd */ nothing);

            return build_proc.join();
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

        int run_clean()
        {
            // Use 'change to project root' to find build temporaries
            scoped_project_root srp;

            remove_all("pkg");
            remove_all(".boc-cache");

            return EXIT_SUCCESS;
        }

        std::set<path> get_coverage_exclusions()
        {
            std::set<path> rv;

            project_file pf("project.json");

            for(auto const &program : pf.programs) {
                if(program.second->exclude_test_coverage) {
                    rv.insert(program.second->source_directory);
                }
            }

            for(auto const &library : pf.libraries) {
                if(library.second->exclude_test_coverage) {
                    rv.insert(library.second->source_directory);
                }
            }

            return rv;
        }

        int run_coverage_report()
        {
            LOG_INFO("Generating coverage report");
            scoped_project_root srp;

            std::vector<std::string> args {
                "--root", canonical(current_path()).generic_string(),
                "--object-directory", canonical(current_path()).generic_string(),
                "--sort-uncovered",
                "--exclude-unreachable-branches"
            };

            auto coverage_exclusions = get_coverage_exclusions();
            for(auto const &excl : coverage_exclusions) {
                args.push_back("--exclude");
                args.push_back(excl.generic_string());
            }

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

            std::vector<std::string> args {
                "--gcov-options", "\\-lp",
                "--root", canonical(current_path()).generic_string(),
                "--build-root", canonical(current_path()).generic_string()
            };

            auto coverage_exclusions = get_coverage_exclusions();
            for(auto const &excl : coverage_exclusions) {
                args.push_back("--exclude");
                args.push_back(excl.generic_string());
            }

            process report_proc("cpp-coveralls",
                                args,
                                /* stdin */ nothing,
                                /* stdout */ nothing,
                                /* stderr */ nothing,
                                /* cwd */ nothing);

            return report_proc.join();
        }
    };

}

MAKE_MAIN(gorc::boc_frontend_program)

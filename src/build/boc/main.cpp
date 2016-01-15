#include "program/program.hpp"
#include "build/common/paths.hpp"
#include "utility/join.hpp"
#include "system/self.hpp"
#include "system/process.hpp"
#include "subcommand_type.hpp"
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
    };

}

MAKE_MAIN(gorc::boc_frontend_program)

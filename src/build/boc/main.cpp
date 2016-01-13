#include "program/program.hpp"
#include "build/common/paths.hpp"
#include "utility/join.hpp"
#include "system/self.hpp"
#include "system/process.hpp"
#include <boost/filesystem.hpp>
#include <regex>

using namespace boost::filesystem;

namespace gorc {

    class boc_frontend_program : public program {
    public:
        bool no_progress = false;

        virtual void create_options(options &opt) override
        {
            opt.insert(make_switch_option("no-progress", no_progress));
        }

        std::vector<std::string> construct_common_args() const
        {
            std::vector<std::string> rv;

            if(no_progress) {
                rv.push_back("--no-progress");
            }

            return rv;
        }

        virtual int main() override
        {
            return run_autodetect();
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

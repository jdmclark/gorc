#include "program/program.hpp"
#include "io/path.hpp"
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

namespace gorc {

    class boc_build_program : public program {
    public:
        std::string boc_root_filename = "project.json";
        bool list_only = false;

        path original_working_directory;
        path project_root_path;

        virtual void create_options(options &opts) override
        {
            // Test and debugging options
            opts.insert(make_value_option("override-root-name", boc_root_filename));
            opts.insert(make_switch_option("list", list_only));
            return;
        }

        void change_to_project_root()
        {
            while(true) {
                if(is_regular_file(boc_root_filename)) {
                    project_root_path = canonical(current_path());
                    return;
                }

                auto canonical_current_path = canonical(current_path());

                if(!canonical_current_path.has_parent_path()) {
                    LOG_FATAL("current directory is not a boc project");
                }

                current_path(canonical_current_path.parent_path());
            }
        }

        virtual int main() override
        {
            original_working_directory = canonical(current_path());
            change_to_project_root();

            return EXIT_SUCCESS;
        }
    };

}

MAKE_MAIN(gorc::boc_build_program)

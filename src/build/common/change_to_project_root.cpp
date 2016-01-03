#include "change_to_project_root.hpp"
#include "log/log.hpp"
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

void gorc::change_to_project_root(path const &boc_root_filename,
                                  path &out_original_working_directory,
                                  path &out_project_root,
                                  path &out_original_working_directory_rel)
{
    out_original_working_directory = canonical(current_path());

    path leftover = ".";
    while(true) {
        if(is_regular_file(boc_root_filename)) {
            break;
        }

        auto canonical_current_path = canonical(current_path());

        if(!canonical_current_path.has_parent_path()) {
            LOG_FATAL("current directory is not a boc project");
        }

        leftover = canonical_current_path.filename() / leftover;
        current_path(canonical_current_path.parent_path());
    }

    out_project_root = canonical(current_path());
    out_original_working_directory_rel = leftover.normalize();
}

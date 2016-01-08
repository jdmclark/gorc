#pragma once

#include "io/path.hpp"

namespace gorc {

    void change_to_project_root(path &out_original_working_directory,
                                path &out_project_root,
                                path &out_original_working_directory_rel);

}

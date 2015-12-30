#pragma once

#include "io/path.hpp"
#include <unordered_map>
#include <memory>
#include <set>

namespace gorc {

    class project_file {
    public:
        std::set<path> project_files;

        project_file(path const &filename);
    };

}

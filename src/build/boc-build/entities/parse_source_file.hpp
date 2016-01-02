#pragma once

#include <set>
#include <vector>
#include "io/path.hpp"
#include "utility/maybe.hpp"

namespace gorc {

    maybe<std::set<path>> get_source_file_headers(path const &p,
                                                  std::vector<path> const &search_dirs);

}

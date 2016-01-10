#pragma once

#include <set>
#include "io/path.hpp"
#include "utility/service_registry.hpp"

namespace gorc {

    int run_tests(std::set<path> const &tests,
                  path const &boc_shell,
                  path const &fail_log,
                  service_registry const &,
                  bool print_summary);

}

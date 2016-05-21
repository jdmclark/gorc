#pragma once

#include "utility/service_registry.hpp"
#include "io/path.hpp"

namespace gorc {

    class root_entity;

    int run_build(service_registry const &,
                  root_entity*,
                  size_t threads,
                  bool print_summary,
                  path const &working_dir);

}

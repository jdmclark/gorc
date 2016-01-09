#pragma once

#include "utility/service_registry.hpp"

namespace gorc {

    class root_entity;

    int run_build(service_registry const &,
                  root_entity*,
                  bool print_summary);

}

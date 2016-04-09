#pragma once

#include <unordered_map>
#include <string>
#include "value.hpp"

namespace gorc {
    namespace cog {

        using constant_table = std::unordered_map<std::string, value>;

        void default_populate_constant_table(constant_table &);

    }
}

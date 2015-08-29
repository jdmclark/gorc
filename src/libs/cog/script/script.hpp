#pragma once

#include "symbol_table.hpp"
#include "string_table.hpp"

namespace gorc {
    namespace cog {

        class script {
        public:
            symbol_table symbols;
            string_table strings;
        };

    }
}

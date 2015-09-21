#pragma once

#include "symbol_table.hpp"
#include "string_table.hpp"
#include "message_table.hpp"
#include "io/memory_file.hpp"

namespace gorc {
    namespace cog {

        class script {
        public:
            symbol_table symbols;
            string_table strings;
            message_table exports;
            memory_file program;
        };

    }
}

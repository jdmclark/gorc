#pragma once

#include "cog/script/script.hpp"
#include "cog/script/verb_table.hpp"
#include "heap.hpp"

namespace gorc {
    namespace cog {

        class vm {
        public:
            void execute(verb_table &, script const &, heap &, size_t pc);
        };

    }
}

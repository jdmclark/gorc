#pragma once

#include "cog/script/verb_table.hpp"
#include "continuation.hpp"

namespace gorc {
    namespace cog {

        class vm {
        public:
            void execute(verb_table &, service_registry &, continuation &cc);
        };

    }
}

#pragma once

#include "continuation.hpp"
#include "heap.hpp"
#include "jk/cog/script/verb_table.hpp"

namespace gorc {
    namespace cog {

        class executor;

        class virtual_machine {
        private:
            value internal_execute(heap &globals,
                                   verb_table &,
                                   executor &,
                                   service_registry &,
                                   continuation &cc);

        public:
            value execute(heap &globals,
                          verb_table &,
                          executor &,
                          service_registry &,
                          continuation &cc);
        };
    }
}

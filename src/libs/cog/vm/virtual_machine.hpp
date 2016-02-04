#pragma once

#include "cog/script/verb_table.hpp"
#include "continuation.hpp"

namespace gorc {
    namespace cog {

        class executor;

        class virtual_machine {
        private:
            value internal_execute(verb_table &, executor &, service_registry &, continuation &cc);

        public:
            value execute(verb_table &, executor &, service_registry &, continuation &cc);
        };

    }
}

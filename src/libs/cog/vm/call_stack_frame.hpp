#pragma once

#include "cog/script/script.hpp"
#include "heap.hpp"
#include <cstddef>

namespace gorc {
    namespace cog {

        class call_stack_frame {
        public:
            script const &cog;
            heap &memory;
            size_t program_counter;

            call_stack_frame(script const &cog,
                             heap &memory,
                             size_t program_counter);
        };

    }
}

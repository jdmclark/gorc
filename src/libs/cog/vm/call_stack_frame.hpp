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
            value sender;
            value source;
            value param0;
            value param1;
            value param2;
            value param3;

            value return_register;

            // Apply return register to next frame on return.
            bool save_return_register = true;

            // Push return register after frame is popped.
            bool push_return_register = false;

            call_stack_frame(script const &cog,
                             heap &memory,
                             size_t program_counter,
                             value sender,
                             value source,
                             value param0,
                             value param1,
                             value param2,
                             value param3);
        };

    }
}

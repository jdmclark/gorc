#pragma once

#include <stack>
#include "cog/script/stack.hpp"
#include "call_stack_frame.hpp"
#include "cog/script/value.hpp"

namespace gorc {
    namespace cog {

        class continuation {
        public:
            std::stack<call_stack_frame> call_stack;
            cog::stack data_stack;

            continuation() = default;
            continuation(call_stack_frame &&frame);

            call_stack_frame& frame();
        };

    }
}

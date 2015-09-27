#pragma once

#include <stack>
#include "call_stack_frame.hpp"
#include "cog/script/value.hpp"

namespace gorc {
    namespace cog {

        class continuation {
        public:
            std::stack<call_stack_frame> call_stack;
            std::stack<value> data_stack;

            continuation(call_stack_frame &&frame);
        };

    }
}

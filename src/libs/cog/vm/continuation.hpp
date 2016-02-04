#pragma once

#include <vector>
#include "cog/script/stack.hpp"
#include "call_stack_frame.hpp"
#include "cog/script/value.hpp"
#include "io/binary_input_stream.hpp"
#include "io/binary_output_stream.hpp"

namespace gorc {
    namespace cog {

        class continuation {
        public:
            std::vector<call_stack_frame> call_stack;
            cog::stack data_stack;

            continuation() = default;
            explicit continuation(call_stack_frame &&frame);

            continuation(deserialization_constructor_tag, binary_input_stream &);
            void binary_serialize_object(binary_output_stream &) const;

            call_stack_frame& frame();
        };

    }
}

#pragma once

#include "jk/cog/script/script.hpp"
#include "heap.hpp"
#include "content/asset_ref.hpp"
#include "io/binary_input_stream.hpp"
#include "io/binary_output_stream.hpp"
#include <cstddef>

namespace gorc {
    namespace cog {

        class call_stack_frame {
        public:
            cog_id instance_id;
            size_t program_counter;
            value sender;
            value sender_id;
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

            call_stack_frame(cog_id instance_id,
                             size_t program_counter,
                             value sender,
                             value sender_id,
                             value source,
                             value param0,
                             value param1,
                             value param2,
                             value param3);

            call_stack_frame(deserialization_constructor_tag, binary_input_stream &);
            void binary_serialize_object(binary_output_stream &) const;
        };

    }
}

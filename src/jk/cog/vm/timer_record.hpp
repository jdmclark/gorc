#pragma once

#include "io/binary_input_stream.hpp"
#include "io/binary_output_stream.hpp"
#include "jk/cog/script/value.hpp"

namespace gorc {
    namespace cog {

        class timer_record {
        public:
            time_delta duration;
            time_delta remaining;
            value param0;
            value param1;

            timer_record(time_delta const &duration,
                         value param0,
                         value param1);

            timer_record(deserialization_constructor_tag, binary_input_stream &bis);

            void binary_serialize_object(binary_output_stream &bos) const;
        };

    }
}

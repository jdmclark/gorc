#pragma once

#include "io/binary_input_stream.hpp"
#include "io/binary_output_stream.hpp"

namespace gorc {
    namespace cog {

        class pulse_record {
        public:
            time_delta duration;
            time_delta remaining;

            explicit pulse_record(time_delta const &duration);

            pulse_record(deserialization_constructor_tag, binary_input_stream &bis);

            void binary_serialize_object(binary_output_stream &bos) const;
        };

    }
}

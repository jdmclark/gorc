#pragma once

#include "continuation.hpp"
#include "utility/time.hpp"
#include "io/binary_input_stream.hpp"
#include "io/binary_output_stream.hpp"

namespace gorc {
    namespace cog {

        class sleep_record {
        public:
            continuation cc;
            time_delta expiration_time;

            sleep_record(continuation &&cc,
                         time_delta expiration_time);

            sleep_record(deserialization_constructor_tag, binary_input_stream &bis);

            void binary_serialize_object(binary_output_stream &bos) const;
        };

    }
}

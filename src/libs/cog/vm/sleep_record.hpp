#pragma once

#include "continuation.hpp"
#include "utility/time.hpp"

namespace gorc {
    namespace cog {

        class sleep_record {
        public:
            continuation cc;
            time_delta expiration_time;

            sleep_record(continuation &&cc,
                         time_delta expiration_time);
        };

    }
}

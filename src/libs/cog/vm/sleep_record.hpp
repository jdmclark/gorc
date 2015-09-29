#pragma once

#include "continuation.hpp"

namespace gorc {
    namespace cog {

        class sleep_record {
        public:
            continuation cc;
            double expiration_time;

            sleep_record(continuation &&cc,
                         double expiration_time);
        };

    }
}

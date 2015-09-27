#pragma once

#include "cog/script/script.hpp"
#include "heap.hpp"

namespace gorc {
    namespace cog {

        class instance {
        public:
            script const &cog;
            heap memory;

            instance(script const &);
        };

    }
}

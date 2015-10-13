#pragma once

#include "cog/script/script.hpp"
#include "heap.hpp"
#include <vector>

namespace gorc {
    namespace cog {

        class instance {
        public:
            script const &cog;
            heap memory;

            instance(script const &);
            instance(script const &, std::vector<value> const &);
        };

    }
}

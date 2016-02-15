#pragma once

#include "utility/time.hpp"

namespace gorc {

    class aspect {
    public:
        virtual ~aspect();
        virtual void update(time_delta) = 0;
    };

}

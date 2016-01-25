#pragma once

#include "utility/time.hpp"

namespace gorc {

    class input_bindable_command {
    public:
        virtual ~input_bindable_command();

        virtual void update(time_delta) = 0;
    };

}

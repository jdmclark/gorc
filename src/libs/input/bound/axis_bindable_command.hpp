#pragma once

#include "input_bindable_command.hpp"

namespace gorc {

    class axis_bindable_command : public input_bindable_command {
    public:
        virtual ~axis_bindable_command();

        virtual void add_value(float value) = 0;
    };

}

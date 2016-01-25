#pragma once

#include "input_bindable_command.hpp"

namespace gorc {

    class button_bindable_command : public input_bindable_command {
    public:
        virtual ~button_bindable_command();

        virtual void set_pressed() = 0;
    };

}

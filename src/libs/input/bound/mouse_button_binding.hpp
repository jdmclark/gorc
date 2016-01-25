#pragma once

#include "input_binding.hpp"
#include "button_bindable_command.hpp"

namespace gorc {

    class mouse_button_binding : public input_binding {
    private:
        mouse_button bound_button;
        button_bindable_command &command;

    public:
        mouse_button_binding(button_bindable_command &command, mouse_button bound_button);

        virtual void handle_mouse_input(time_delta, mouse&) override;
    };

}

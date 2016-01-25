#pragma once

#include "input_binding.hpp"
#include "button_bindable_command.hpp"

namespace gorc {

    class keyboard_button_binding : public input_binding {
    private:
        keyboard_key bound_key;
        button_bindable_command &command;

    public:
        keyboard_button_binding(button_bindable_command &command, keyboard_key bound_key);

        virtual void handle_keyboard_input(time_delta, keyboard&) override;
    };

}

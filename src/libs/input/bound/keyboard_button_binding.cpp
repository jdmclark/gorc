#include "keyboard_button_binding.hpp"

gorc::keyboard_button_binding::keyboard_button_binding(button_bindable_command &command,
                                                       keyboard_key bound_key)
    : bound_key(bound_key)
    , command(command)
{
    return;
}

void gorc::keyboard_button_binding::handle_keyboard_input(time_delta, keyboard &kb)
{
    if(kb.is_key_down(bound_key)) {
        command.set_pressed();
    }
}

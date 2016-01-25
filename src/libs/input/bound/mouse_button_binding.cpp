#include "mouse_button_binding.hpp"

gorc::mouse_button_binding::mouse_button_binding(button_bindable_command &command,
                                                 mouse_button bound_button)
    : bound_button(bound_button)
    , command(command)
{
    return;
}

void gorc::mouse_button_binding::handle_mouse_input(time_delta, mouse &m)
{
    if(m.is_button_down(bound_button)) {
        command.set_pressed();
    }
}

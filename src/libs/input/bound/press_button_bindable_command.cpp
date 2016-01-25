#include "press_button_bindable_command.hpp"

gorc::press_button_bindable_command::press_button_bindable_command(
        std::function<void(time_delta)> &&on_press)
    : on_press(std::forward<std::function<void(time_delta)>>(on_press))
{
    return;
}

void gorc::press_button_bindable_command::set_pressed()
{
    curr_state = true;
}

void gorc::press_button_bindable_command::update(time_delta t)
{
    if(curr_state && !prev_state) {
        on_press(t);
    }

    prev_state = curr_state;
    curr_state = false;
}

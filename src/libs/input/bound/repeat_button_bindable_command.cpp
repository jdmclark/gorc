#include "repeat_button_bindable_command.hpp"

gorc::repeat_button_bindable_command::repeat_button_bindable_command(
        time_delta repeat_delay,
        time_delta repeat_time,
        std::function<void(time_delta)> &&on_press)
    : on_press(std::forward<std::function<void(time_delta)>>(on_press))
    , repeat_delay(repeat_delay)
    , repeat_time(repeat_time)
{
    return;
}

void gorc::repeat_button_bindable_command::set_pressed()
{
    curr_state = true;
}

void gorc::repeat_button_bindable_command::update(time_delta t)
{
    if(curr_state && !prev_state) {
        on_press(t);
        current_time = repeat_delay;
    }

    if(curr_state) {
        current_time -= t;
        while(current_time <= 0.0s) {
            current_time += repeat_time;
            on_press(t);
        }
    }

    prev_state = curr_state;
    curr_state = false;
}

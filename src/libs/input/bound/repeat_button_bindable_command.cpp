#include "repeat_button_bindable_command.hpp"

gorc::repeat_button_bindable_command::repeat_button_bindable_command(
        double repeat_delay,
        double repeat_time,
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
        current_time -= t.count();
        while(current_time <= 0.0) {
            current_time += repeat_time;
            on_press(t);
        }
    }

    prev_state = curr_state;
    curr_state = false;
}

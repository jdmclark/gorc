#include "linear_axis_bindable_command.hpp"

gorc::linear_axis_bindable_command::linear_axis_bindable_command(
        std::function<void(time_delta, float)> &&fn)
    : on_update(std::forward<std::function<void(time_delta, float)>>(fn))
{
    return;
}

void gorc::linear_axis_bindable_command::add_value(float dv)
{
    value += dv;
}

void gorc::linear_axis_bindable_command::update(time_delta t)
{
    on_update(t, value);
    value = 0.0f;
}

#include "mouse_axis_binding.hpp"

gorc::mouse_axis_binding::mouse_axis_binding(axis_bindable_command &command,
                                              vector<2, float> const &direction)
    : command(command)
    , direction(direction)
{
    return;
}

void gorc::mouse_axis_binding::handle_mouse_velocity(time_delta, vector<2, float> const &vel)
{
    command.add_value(dot(vel, direction));
}

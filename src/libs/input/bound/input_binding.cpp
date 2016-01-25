#include "input_binding.hpp"

gorc::input_binding::~input_binding()
{
    return;
}

void gorc::input_binding::handle_mouse_input(time_delta, mouse&)
{
    return;
}

void gorc::input_binding::handle_keyboard_input(time_delta, keyboard&)
{
    return;
}

void gorc::input_binding::handle_mouse_velocity(time_delta, vector<2, float> const &)
{
    return;
}

#include "bound_input_adapter.hpp"

void gorc::bound_input_adapter::handle_mouse_cursor(time_delta t, mouse &m)
{
    auto window_sz = m.get_cursor_area();
    point<2, float> cursor_origin = make_point(static_cast<float>(get<0>(window_sz) / 2),
                                               static_cast<float>(get<1>(window_sz) / 2));
    auto cursor_pos = static_cast<point<2, float>>(m.get_cursor_position());

    input_was_skipped = false;
    if(accept_cursor) {
        // Report cursor in FOVs per second
        auto cursor_s = (cursor_pos - cursor_origin) /
                        (static_cast<float>(get<0>(window_sz)) *
                         static_cast<float>(t.count()));
        for(auto &binding : bindings) {
            binding->handle_mouse_velocity(t, cursor_s);
        }
    }

    accept_cursor = true;

    m.set_cursor_visible(false);
    m.set_cursor_position(static_cast<point<2, int>>(cursor_origin));
}

bool gorc::bound_input_adapter::wants_mouse_focus()
{
    if(input_was_skipped) {
        // Input from previous frame was skipped.
        // Put cursor in initialization state. This prevents aberrant cursor inputs when the
        // input mode is switched between bound and gui.
        accept_cursor = false;
    }

    input_was_skipped = true;
    return true;
}

bool gorc::bound_input_adapter::wants_keyboard_focus()
{
    return true;
}

void gorc::bound_input_adapter::handle_mouse_input(time_delta t, mouse& m)
{
    for(auto &binding : bindings) {
        binding->handle_mouse_input(t, m);
    }
}

void gorc::bound_input_adapter::handle_keyboard_input(time_delta t, keyboard& k)
{
    for(auto &binding : bindings) {
        binding->handle_keyboard_input(t, k);
    }
}

void gorc::bound_input_adapter::handle_text_input(time_delta, char)
{
    // Text input deliberately cannot be bound
    return;
}

void gorc::bound_input_adapter::update(time_delta t)
{
    for(auto &cmd : button_commands) {
        cmd.second->update(t);
    }

    for(auto &cmd : axis_commands) {
        cmd.second->update(t);
    }
}

#include "mock_mouse.hpp"

bool mock_mouse::is_button_down(gorc::mouse_button) const
{
    return false;
}

gorc::point<2, int> mock_mouse::get_cursor_position() const
{
    return gorc::make_origin_point<2, int>();
}

bool mock_mouse::get_cursor_visible() const
{
    return false;
}

void mock_mouse::set_cursor_position(gorc::point<2, int> const &)
{
    return;
}

void mock_mouse::set_cursor_visible(bool)
{
    return;
}

gorc::size<2, int> mock_mouse::get_cursor_area() const
{
    return gorc::make_size(0, 0);
}

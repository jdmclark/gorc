#pragma once

#include "input/mouse.hpp"

class mock_mouse : public gorc::mouse {
public:
    virtual bool is_button_down(gorc::mouse_button) const override;
    virtual gorc::point<2, int> get_cursor_position() const override;
    virtual bool get_cursor_visible() const override;

    virtual void set_cursor_position(gorc::point<2, int> const &) override;
    virtual void set_cursor_visible(bool) override;

    virtual gorc::size<2, int> get_cursor_area() const override;
};

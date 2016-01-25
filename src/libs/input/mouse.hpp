#pragma once

#include <string>
#include "math/point.hpp"
#include "math/size.hpp"

namespace gorc {

    enum class mouse_button {
        unknown = -1,
        left = 0,
        right,
        middle,
        xbutton1,
        xbutton2,

        count
    };

    mouse_button string_to_mouse_button(std::string const &);
    std::string const &mouse_button_to_string(mouse_button);

    class mouse {
    public:
        virtual ~mouse();

        virtual bool is_button_down(mouse_button) const = 0;
        virtual point<2, int> get_cursor_position() const = 0;
        virtual bool get_cursor_visible() const = 0;

        virtual void set_cursor_position(point<2, int> const &) = 0;
        virtual void set_cursor_visible(bool) = 0;

        virtual size<2, int> get_cursor_area() const = 0;
    };

}

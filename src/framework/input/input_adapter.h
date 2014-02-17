#pragma once

#include "framework/math/vector.h"
#include "framework/utility/time.h"

namespace gorc {
namespace input {

class input_adapter {
public:
    virtual ~input_adapter();

    virtual void set_mouse_cursor_position(const time& time, const vector<2, int>& cursor_pos) = 0;

    virtual bool wants_mouse_focus() const = 0;
    virtual bool hide_mouse_cursor() const = 0;
    virtual bool wants_keyboard_focus() const = 0;

    virtual void handle_mouse_input(const time& time) = 0;
    virtual void handle_keyboard_input(const time& time) = 0;
    virtual void handle_text_input(const time& time, char ch) = 0;
};

}
}

#pragma once

#include "mouse.hpp"
#include "keyboard.hpp"
#include "utility/time.hpp"

namespace gorc {

    class input_adapter {
    public:
        virtual ~input_adapter();

        virtual void handle_mouse_cursor(time_delta, mouse&) = 0;

        virtual bool wants_mouse_focus() = 0;
        virtual bool wants_keyboard_focus() = 0;

        virtual void handle_mouse_input(time_delta, mouse&) = 0;
        virtual void handle_keyboard_input(time_delta, keyboard&) = 0;
        virtual void handle_text_input(time_delta, char) = 0;
    };

}

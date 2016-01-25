#pragma once

#include "input/mouse.hpp"
#include "input/keyboard.hpp"
#include "utility/time.hpp"
#include "math/vector.hpp"

namespace gorc {

    class input_binding {
    public:
        virtual ~input_binding();

        virtual void handle_mouse_input(time_delta, mouse&);
        virtual void handle_keyboard_input(time_delta, keyboard&);
        virtual void handle_mouse_velocity(time_delta, vector<2, float> const &);
    };

}

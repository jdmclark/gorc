#pragma once

#include "framework/utility/time.h"
#include <array>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

namespace gorc {
namespace input {

class gui_input_mouse_button_state {
public:
    bool pressed = false;
};

template <typename ViewT> class mouse_click_dispatcher {
private:
    ViewT& view;
    vector<2, int> cursor_pos = make_vector(0, 0);

    std::array<gui_input_mouse_button_state, sf::Mouse::ButtonCount> mouse_state;

public:
    mouse_click_dispatcher(ViewT& view) : view(view) {
        return;
    }

    void set_mouse_cursor_position(const time&, const vector<2, int>& cursor_pos) {
        this->cursor_pos = cursor_pos;
    }

    void handle_mouse_input(const time& time) {
        for(int i = 0; i < sf::Mouse::ButtonCount; ++i) {
            sf::Mouse::Button b = static_cast<sf::Mouse::Button>(i);
            bool is_now_pressed = sf::Mouse::isButtonPressed(static_cast<sf::Mouse::Button>(b));
            gui_input_mouse_button_state& cur_state = mouse_state[b];

            if(cur_state.pressed && !is_now_pressed) {
                view.on_mouse_button_up(time, cursor_pos, b);
            }
            else if(!cur_state.pressed && is_now_pressed) {
                view.on_mouse_button_down(time, cursor_pos, b);
            }

            cur_state.pressed = is_now_pressed;
        }
    }
};

}
}

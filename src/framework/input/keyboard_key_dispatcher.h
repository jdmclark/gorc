#pragma once

#include "framework/utility/time.h"
#include <array>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

namespace gorc {
namespace input {

constexpr double gui_input_key_repeat_delay = 0.5;
constexpr double gui_input_key_repeat_time = 0.05;

class gui_input_keyboard_key_state {
public:
    bool pressed = false;
    double pressed_time = 0.0;
};

template <typename ViewT, bool do_repeat = true> class keyboard_key_dispatcher {
private:
    ViewT& view;
    std::array<gui_input_keyboard_key_state, sf::Keyboard::KeyCount> key_state;

public:
    keyboard_key_dispatcher(ViewT& view) : view(view) {
        return;
    }

    void handle_keyboard_input(const time& time) {
        bool shift_down = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);
        bool ctrl_down = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
        bool alt_down = sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt);

        for(int i = 0; i < sf::Keyboard::KeyCount; ++i) {
            sf::Keyboard::Key k = static_cast<sf::Keyboard::Key>(i);
            bool is_now_pressed = sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(k));
            gui_input_keyboard_key_state& cur_state = key_state[k];

            if(cur_state.pressed && !is_now_pressed) {
                view.on_keyboard_key_up(time, k, shift_down, ctrl_down, alt_down);
                cur_state.pressed = false;
            }
            else if(!cur_state.pressed && is_now_pressed) {
                view.on_keyboard_key_down(time, k, shift_down, ctrl_down, alt_down);
                cur_state.pressed = true;
                cur_state.pressed_time = 0.0;
            }
            else if(cur_state.pressed && is_now_pressed && do_repeat) {
                // Handle keyboard repeat
                cur_state.pressed_time += time;

                while(cur_state.pressed_time > gui_input_key_repeat_delay) {
                    cur_state.pressed_time -= gui_input_key_repeat_time;
                    view.on_keyboard_key_down(time, k, shift_down, ctrl_down, alt_down);
                }
            }
        }
    }
};

}
}

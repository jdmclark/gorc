#pragma once

#include "input_adapter.h"
#include "mouse_click_dispatcher.h"
#include "keyboard_key_dispatcher.h"
#include <array>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

namespace gorc {
namespace input {

template <typename ViewT> class gui_input_adapter : public input_adapter {
private:
    mouse_click_dispatcher<ViewT> click_dispatcher;
    keyboard_key_dispatcher<ViewT> key_dispatcher;

    ViewT& view;

public:
    gui_input_adapter(ViewT& view) : click_dispatcher(view), key_dispatcher(view), view(view) {
        return;
    }

    virtual void set_mouse_cursor_position(const time& time, const vector<2, int>& cursor_pos) {
        click_dispatcher.set_mouse_cursor_position(time, cursor_pos);
        view.set_mouse_cursor_position(time, cursor_pos);
    }

    virtual bool wants_mouse_focus() const {
        return view.wants_mouse_focus();
    }

    virtual bool hide_mouse_cursor() const {
        return false;
    }

    virtual bool wants_keyboard_focus() const {
        return view.wants_keyboard_focus();
    }

    virtual void handle_mouse_input(const time& time) {
        if(view.wants_mouse_focus()) {
            click_dispatcher.handle_mouse_input(time);
        }
    }

    virtual void handle_keyboard_input(const time& time) {
        if(view.wants_keyboard_focus()) {
            key_dispatcher.handle_keyboard_input(time);
        }
    }

    virtual void handle_text_input(const time& time, char ch) {
        if(view.wants_keyboard_focus()) {
            view.on_keyboard_text_entered(time, ch);
        }
    }
};

}
}

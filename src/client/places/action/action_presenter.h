#pragma once

#include "client/presenter.h"
#include "framework/input/passthrough_input_adapter.h"
#include "framework/input/keyboard_key_dispatcher.h"
#include "framework/input/mouse_click_dispatcher.h"

namespace gorc {
namespace client {

class application;

namespace action {

class action_presenter : public gorc::client::presenter {
private:
    bool window_has_focus = true;
    application& app;
    input::passthrough_input_adapter<action_presenter> input_adapter;
    input::keyboard_key_dispatcher<action_presenter, false> key_dispatcher;
    input::mouse_click_dispatcher<action_presenter> click_dispatcher;

public:
    action_presenter(application& app);

    virtual void start(event::event_bus&) override;

    inline input::input_adapter& get_input_adapter() {
        return input_adapter;
    }

    inline bool wants_mouse_focus() const {
        return true;
    }

    inline bool hide_mouse_cursor() const {
        return true;
    }

    inline bool wants_keyboard_focus() const {
        return true;
    }

    void handle_mouse_input(const time& time);
    void handle_keyboard_input(const time& time);

    void on_mouse_button_down(const time& time, const vector<2, int>& cursor_pos, sf::Mouse::Button b);
    void on_mouse_button_up(const time& time, const vector<2, int>& cursor_pos, sf::Mouse::Button b);

    void on_keyboard_key_down(const time& time, sf::Keyboard::Key k, bool shift, bool ctrl, bool alt);
    void on_keyboard_key_up(const time& time, sf::Keyboard::Key k, bool shift, bool ctrl, bool alt);
};

}
}
}

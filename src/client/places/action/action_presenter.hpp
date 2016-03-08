#pragma once

#include "client/presenter.hpp"
#include "libold/base/input/passthrough_input_adapter.hpp"
#include "libold/base/input/keyboard_key_dispatcher.hpp"
#include "libold/base/input/mouse_click_dispatcher.hpp"

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

    maybe<scoped_delegate> window_focus_delegate;

public:
    action_presenter(application& app);

    virtual void start(event_bus&) override;

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

    void handle_mouse_input(const gorc::time& time);
    void handle_keyboard_input(const gorc::time& time);

    void on_mouse_button_down(const gorc::time& time, const vector<2, int>& cursor_pos, sf::Mouse::Button b);
    void on_mouse_button_up(const gorc::time& time, const vector<2, int>& cursor_pos, sf::Mouse::Button b);

    void on_keyboard_key_down(const gorc::time& time, sf::Keyboard::Key k, bool shift, bool ctrl, bool alt);
    void on_keyboard_key_up(const gorc::time& time, sf::Keyboard::Key k, bool shift, bool ctrl, bool alt);
};

}
}
}
